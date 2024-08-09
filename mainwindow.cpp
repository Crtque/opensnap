#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <iostream>
#include <windows.h>
HHOOK hHook = NULL;
bool dKeyPressed = false;
bool aKeyPressed = false;
bool wKeyPressed = false;
bool sKeyPressed = false;
WORD KEY_D = 0x44;
WORD KEY_A = 0x41;
WORD KEY_W = 0x57;
WORD KEY_S = 0x53;

bool autoStop = false;

bool dRelProgrammly = false;
bool aRelProgrammly = false;
bool wRelProgrammly = false;
bool sRelProgrammly = false;
WORD StringToKeyCodes(const std::string &text) {
    WORD keyCodes;

    for (char c : text) {
        // Convert character to uppercase because virtual keycodes for A-Z are
        // uppercase only
        c = toupper(c);

        // Check if it's an alphabetical character
        if (c >= 'A' && c <= 'Z') {
            keyCodes = VkKeyScan(c) & 0xFF; // VK_A to VK_Z
        }
        // Check if it's a digit
        else if (c >= '0' && c <= '9') {
            keyCodes = VkKeyScan(c) & 0xFF; // VK_0 to VK_9
        }
        // Handle space character
        else if (c == ' ') {
            keyCodes = VK_SPACE;
        }
        // Handle other special characters
        else {
            // Use VkKeyScan for other special characters
            SHORT vk = VkKeyScan(c);
            if (vk != -1) {
                keyCodes = vk & 0xFF; // Get the low byte
            }
        }
    }

    return keyCodes;
}
int delay = 60;

void KeyPress(WORD keyCode) {
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = keyCode;
    input.ki.dwFlags = 0;
    SendInput(1, &input, sizeof(INPUT));
}

void KeyUp(WORD keyCode) {
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = keyCode;
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}
bool withoutTrigger = false;
LRESULT CALLBACK MainWindow::LowLevelKeyboardProc(int nCode, WPARAM wParam,
                                                  LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT *pKeyBoard = (KBDLLHOOKSTRUCT *)lParam;

        // Обработка нажатий клавиш
        if (wParam == WM_KEYDOWN) {
            if (!withoutTrigger) {
                if (pKeyBoard->vkCode == KEY_D) {
                    if (aKeyPressed && !dKeyPressed) {
                        aRelProgrammly = true;
                        KeyUp(KEY_A);
                    }
                    dKeyPressed = true;
                } else if (pKeyBoard->vkCode == KEY_A) {
                    if (dKeyPressed && !aKeyPressed) {
                        dRelProgrammly = true;
                        KeyUp(KEY_D);
                    }
                    aKeyPressed = true;
                } else if (pKeyBoard->vkCode == KEY_W) {
                    if (sKeyPressed && !wKeyPressed) {
                        sRelProgrammly = true;
                        KeyUp(KEY_S);
                    }
                    wKeyPressed = true;
                } else if (pKeyBoard->vkCode == KEY_S) {
                    if (wKeyPressed && !sKeyPressed) {
                        wRelProgrammly = true;
                        KeyUp(KEY_W);
                    }
                    sKeyPressed = true;
                }
            }
        }

        // Обработка отпускания клавиш
        else if (wParam == WM_KEYUP) {
            if (!withoutTrigger) {
                if (pKeyBoard->vkCode == KEY_D) {
                    if (dRelProgrammly) {
                        dRelProgrammly = false;
                    } else {
                        dKeyPressed = false;
                    }
                    if (aKeyPressed) {
                        KeyPress(KEY_A);
                    } else {
                        if (autoStop) {
                            withoutTrigger = true;
                            KeyUp(KEY_D);
                            KeyPress(KEY_A);
                            Sleep(delay);
                            KeyUp(KEY_A);
                            withoutTrigger = false;
                        }
                    }
                } else if (pKeyBoard->vkCode == KEY_A) {
                    if (aRelProgrammly) {
                        aRelProgrammly = false;
                    } else {
                        aKeyPressed = false;
                    }
                    if (dKeyPressed) {
                        KeyPress(KEY_D);
                    } else {
                        if (autoStop) {
                            withoutTrigger = true;
                            KeyUp(KEY_A);
                            KeyPress(KEY_D);
                            Sleep(delay);
                            KeyUp(KEY_D);
                            withoutTrigger = false;
                        }
                    }
                } else if (pKeyBoard->vkCode == KEY_W) {
                    if (wRelProgrammly) {
                        wRelProgrammly = false;
                    } else {
                        wKeyPressed = false;
                    }
                    if (sKeyPressed) {
                        KeyPress(KEY_S);
                    } else {
                        if (autoStop) {
                            withoutTrigger = true;
                            KeyUp(KEY_W);
                            KeyPress(KEY_S);
                            Sleep(delay);
                            KeyUp(KEY_S);
                            withoutTrigger = false;
                        }
                    }
                } else if (pKeyBoard->vkCode == KEY_S) {
                    if (sRelProgrammly) {
                        sRelProgrammly = false;
                    } else {
                        sKeyPressed = false;
                    }
                    if (wKeyPressed) {
                        KeyPress(KEY_W);
                    } else {
                        if (autoStop) {
                            withoutTrigger = true;
                            KeyUp(KEY_S);
                            KeyPress(KEY_W);
                            Sleep(delay);
                            KeyUp(KEY_W);
                            withoutTrigger = false;
                        }
                    }
                }
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void MainWindow::onStartClicked() {
    SetHook();
    std::cout << "Hook started!" << std::endl;
}

void MainWindow::onStopClicked() {
    ReleaseHook();
    std::cout << "Hook released!" << std::endl;
}

void MainWindow::SetHook() {
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (!hHook) {
        std::cerr << "Failed to install hook!" << std::endl;
    }
}

void MainWindow::ReleaseHook() {
    if (hHook) {
        UnhookWindowsHookEx(hHook);
        hHook = NULL;
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
    ui->label->setText("<a href=\"https://opensnap.tech/\">OpenSnap.tech</a>");
    ui->label->setTextFormat(Qt::RichText);
    ui->label->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->label->setOpenExternalLinks(true);

    ui->label_2->setText("<a href=\"https://t.me/crtque_tech\">Telegram</a>");
    ui->label_2->setTextFormat(Qt::RichText);
    ui->label_2->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->label_2->setOpenExternalLinks(true);

    ui->label_3->setText(
        "<a href=\"https://steamcommunity.com/id/strel0kk/\">Steam</a>");
    ui->label_3->setTextFormat(Qt::RichText);
    ui->label_3->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->label_3->setOpenExternalLinks(true);
}

MainWindow::~MainWindow() {
    ReleaseHook();
    delete ui;
}

void MainWindow::on_pushButton_clicked() {
    SetHook();
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(true);
}

void MainWindow::on_pushButton_2_clicked() {
    ReleaseHook();
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
}

void MainWindow::on_actionQuit_triggered() { QCoreApplication::quit(); }

void MainWindow::on_pushButton_3_clicked() {
    // Retrieve and set the key codes from the textboxes
    KEY_W = ui->lineEditW->text().isEmpty()
                ? 0x57
                : StringToKeyCodes(ui->lineEditW->text().toStdString());
    KEY_A = ui->lineEditW->text().isEmpty()
                ? 0x41
                : StringToKeyCodes(ui->lineEditA->text().toStdString());
    KEY_S = ui->lineEditW->text().isEmpty()
                ? 0x53
                : StringToKeyCodes(ui->lineEditS->text().toStdString());
    KEY_D = ui->lineEditW->text().isEmpty()
                ? 0x44
                : StringToKeyCodes(ui->lineEditD->text().toStdString());
    ReleaseHook();
    SetHook();

    std::cout << "Key codes updated: W=" << std::hex << KEY_W
              << ", A=" << std::hex << KEY_A << ", S=" << std::hex << KEY_S
              << ", D=" << std::hex << KEY_D << std::endl;

    QMessageBox Msgbox;
    Msgbox.setWindowTitle("Success");
    Msgbox.setText("Key's settings successfully updated");
    Msgbox.exec();
}

void MainWindow::on_lineEditD_2_textChanged(const QString &arg1) {
    delay = ui->lineEditDelay->text().toInt();
}

void MainWindow::on_checkBoxAutostop_checkStateChanged(const Qt::CheckState &arg1)
{
    if (ui->checkBoxAutostop->isChecked()) {
        autoStop = true;
    } else {
        autoStop = false;
    }
    ui->lineEditDelay->setEnabled(autoStop);
}

