#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

#define CONFIG_FILE "adb_config.cfg"

char string[512];
char current_device[50] = "";  // Armazena o ID do dispositivo conectado
char current_model[50] = "";   // Armazena o modelo do dispositivo para exibir no título

void menu_principal(void);
void menu_reboot(void);
void creditos(void);
void clear(void);
void pause(void);
void exibir_dispositivos(void);
void menu_transferencias(void);
void menu_transferir_pcandroid(void);
void menu_transferir_androidpc(void);
void menu_espelhartela(void);
void conectar_dispositivo(void);
void desconectar_dispositivo(void);
void salvar_configuracao(void);
void carregar_configuracao(void);
void atualizar_titulo_janela(void);

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "Portuguese");
    carregar_configuracao();
    atualizar_titulo_janela();
    system("@echo off");
    if (system("adb version") != 0) {
        creditos();
        printf("Erro: ADB nao encontrado. Por favor, inclua-o no diretorio desta aplicacao.\n\n");
        printf("Download: https://developer.android.com/studio/releases/platform-tools\n\n\n");
        pause();
        return 1;
    }
    
    if (system("scrcpy --version") != 0) {
        creditos();
        printf("Erro: scrcpy nao encontrado. Por favor, inclua todos os arquivos do scrcpy no diretorio desta aplicacao.\n\n");
        printf("Download: https://github.com/Genymobile/scrcpy\n\n\n");
        pause();
        return 1;
    }
    system("@echo on");
    menu_principal();
    clear();
    return 0;
}

void atualizar_titulo_janela() {
    char title[100];
    if (strlen(current_device)) {
        snprintf(title, sizeof(title), "title adb-menu - Dispositivo: %s", current_model);
    } else {
        snprintf(title, sizeof(title), "title adb-menu - Nenhum dispositivo conectado");
    }
    system(title);
}

void salvar_configuracao() {
    FILE *file = fopen(CONFIG_FILE, "w");
    if (file) {
        fprintf(file, "%s\n", current_device);
        fprintf(file, "%s\n", current_model);
        fclose(file);
    }
}

void carregar_configuracao() {
    FILE *file = fopen(CONFIG_FILE, "r");
    if (file) {
        fgets(current_device, sizeof(current_device), file);
        current_device[strcspn(current_device, "\n")] = '\0';
        
        fgets(current_model, sizeof(current_model), file);
        current_model[strcspn(current_model, "\n")] = '\0';
        
        fclose(file);
        
        // Verifica se o dispositivo ainda está conectado
        if (strlen(current_device)) {
            system("adb devices -l > temp_adb.txt");
            FILE *adb_file = fopen("temp_adb.txt", "r");
            if (adb_file) {
                char line[256];
                int found = 0;
                while (fgets(line, sizeof(line), adb_file)) {
                    if (strstr(line, current_device) && strstr(line, "device")) {
                        found = 1;
                        break;
                    }
                }
                fclose(adb_file);
                remove("temp_adb.txt");
                
                if (!found) {
                    current_device[0] = '\0';
                    current_model[0] = '\0';
                }
            }
        }
    }
}

void conectar_dispositivo() {
    clear();
    exibir_dispositivos();
    
    if (strlen(current_device)) {
        printf("\nJa conectado ao dispositivo: %s (%s)\n", current_model, current_device);
        printf("Deseja conectar a outro dispositivo? (s/n): ");
        char resposta;
        scanf("%c", &resposta);
        getchar();
        
        if (resposta != 's' && resposta != 'S') {
            return;
        }
    }
    
    printf("\nDigite o ID do dispositivo (transport_id) para conectar: ");
    char id[50];
    fgets(id, sizeof(id), stdin);
    id[strcspn(id, "\n")] = '\0';
    
    // Verificar se o dispositivo existe
    system("adb devices -l > temp_adb.txt");
    FILE *file = fopen("temp_adb.txt", "r");
    if (file) {
        char line[256];
        int found = 0;
        while (fgets(line, sizeof(line), file)) {
            if (strstr(line, id) && strstr(line, "device")) {
                char serial[50], model[50];
                sscanf(line, "%*s device product:%*s model:%s device:%*s transport_id:%s", model, serial);
                
                if (strcmp(serial, id) == 0) {
                    strncpy(current_device, serial, sizeof(current_device));
                    strncpy(current_model, model, sizeof(current_model));
                    found = 1;
                    break;
                }
            }
        }
        fclose(file);
        remove("temp_adb.txt");
        
        if (found) {
            printf("\nConectado ao dispositivo: %s (%s)\n", current_model, current_device);
            salvar_configuracao();
            atualizar_titulo_janela();
        } else {
            printf("\nDispositivo com ID %s nao encontrado ou nao esta conectado.\n", id);
            current_device[0] = '\0';
            current_model[0] = '\0';
        }
    }
}

void desconectar_dispositivo() {
    if (strlen(current_device)) {
        printf("\nDesconectando do dispositivo: %s (%s)\n", current_model, current_device);
        current_device[0] = '\0';
        current_model[0] = '\0';
        salvar_configuracao();
        atualizar_titulo_janela();
    } else {
        printf("\nNenhum dispositivo conectado para desconectar.\n");
    }
}

void menu_espelhartela(void) {
    if (strlen(current_device)) {
        char command[512];
        char serial[50] = "";
        
        // Comando para obter o serial do dispositivo usando o transport_id
        snprintf(command, sizeof(command), "adb -t %s get-serialno > temp_serial.txt", current_device);
        system(command);
        
        FILE *file = fopen("temp_serial.txt", "r");
        if (file) {
            fgets(serial, sizeof(serial), file);
            serial[strcspn(serial, "\n")] = '\0';
            fclose(file);
            remove("temp_serial.txt");
            
            if (strlen(serial)) {
                snprintf(command, sizeof(command), "start scrcpy-noconsole.vbs -s %s", serial);
                system(command);
            } else {
                printf("Nao foi possivel obter o serial do dispositivo.\n");
            }
        } else {
            printf("Erro ao obter informacoes do dispositivo.\n");
        }
    }
}

void menu_transferencias(void) {
    int escolha;
    printf("\n1- Transferir arquivos deste computador para o Android\n");
    printf("2- Transferir arquivos do Android para este computador\n");
    printf("0- Voltar ao menu principal\n");
    printf("\nEscolha a opcao desejada: ");
    scanf("%d", &escolha);
    getchar();
    switch(escolha) {
        case 1:
            menu_transferir_pcandroid();
            break;
        case 2:
            menu_transferir_androidpc();
            break;
        case 0:
            return;
        default:
            printf("Opcao invalida!\n");
    }
}

void menu_transferir_pcandroid(void) {
    char caminhoOrig[256];
    char caminhoDest[256];
    int escolha;

    printf("\nInsira o caminho de origem (ou 0 para cancelar): ");
    fgets(caminhoOrig, sizeof(caminhoOrig), stdin);
    caminhoOrig[strcspn(caminhoOrig, "\n")] = '\0';
    
    if (strcmp(caminhoOrig, "0") == 0) {
        return;
    }

    printf("\n1- Para o diretorio raiz do Android\n");
    printf("2- Inserir outro diretorio\n");
    printf("0- Cancelar\n");
    printf("\nEscolha uma opcao para mover: ");
    scanf("%d", &escolha);
    getchar();

    switch(escolha) {
        case 1:
            printf("\nAguarde . . .\n\n");
            if (strlen(current_device)) {
                snprintf(string, sizeof(string), "adb -t %s push \"%s\" /sdcard/", current_device, caminhoOrig);
            } else {
                snprintf(string, sizeof(string), "adb push \"%s\" /sdcard/", caminhoOrig);
            }
            system(string);
            pause();
            break;
        case 2:
            printf("Insira o diretorio para o qual deseja transferir o(s) arquivo(s) (ou 0 para cancelar): ");
            fgets(caminhoDest, sizeof(caminhoDest), stdin);
            caminhoDest[strcspn(caminhoDest, "\n")] = '\0';
            if (strcmp(caminhoDest, "0") == 0) {
                return;
            }
            printf("\nAguarde . . .\n\n");
            if (strlen(current_device)) {
                snprintf(string, sizeof(string), "adb -t %s push \"%s\" \"%s\"", current_device, caminhoOrig, caminhoDest);
            } else {
                snprintf(string, sizeof(string), "adb push \"%s\" \"%s\"", caminhoOrig, caminhoDest);
            }
            system(string);
            pause();
            break;
        case 0:
            return;
        default:
            printf("Opcao invalida!\n");
    }
}

void menu_transferir_androidpc(void) {
    char caminhoOrig[256];
    char caminhoDest[512];
    char *userProfile = getenv("USERPROFILE");
    int escolha;

    printf("Insira o caminho de origem, a partir da pasta raiz do Android (ou 0 para cancelar): ");
    fgets(caminhoOrig, sizeof(caminhoOrig), stdin);
    caminhoOrig[strcspn(caminhoOrig, "\n")] = '\0';
    
    if (strcmp(caminhoOrig, "0") == 0) {
        return;
    }

    printf("\n1- Para a pasta Downloads do Windows\n");
    printf("2- Inserir outro diretorio\n");
    printf("0- Cancelar\n");
    printf("\nEscolha uma opcao para mover: ");
    scanf("%d", &escolha);
    getchar();

    switch(escolha) {
        case 1:
            if (userProfile != NULL) {
                snprintf(caminhoDest, sizeof(caminhoDest), "%s\\Downloads", userProfile);
                if (strlen(current_device)) {
                    snprintf(string, sizeof(string), "adb -t %s pull /sdcard/%s \"%s\"", current_device, caminhoOrig, caminhoDest);
                } else {
                    snprintf(string, sizeof(string), "adb pull /sdcard/%s \"%s\"", caminhoOrig, caminhoDest);
                }
                system(string);
            } else {
                printf("Erro ao obter o diretorio do usuario.\n");
            }
            pause();
            break;
        case 2:
            printf("\nInsira o diretorio para o qual deseja transferir o(s) arquivo(s) (ou 0 para cancelar): ");
            fgets(caminhoDest, sizeof(caminhoDest), stdin);
            caminhoDest[strcspn(caminhoDest, "\n")] = '\0';
            if (strcmp(caminhoDest, "0") == 0) {
                return;
            }
            if (strlen(current_device)) {
                snprintf(string, sizeof(string), "adb -t %s pull /sdcard/%s \"%s\"", current_device, caminhoOrig, caminhoDest);
            } else {
                snprintf(string, sizeof(string), "adb pull /sdcard/%s \"%s\"", caminhoOrig, caminhoDest);
            }
            system(string);
            pause();
            break;
        case 0:
            return;
        default:
            printf("\nOpcao invalida!\n");
    }
}

void pause(void) {
    system("pause");
}

void clear(void) {
    system("cls");
}

void creditos(void) {
    clear();
    printf("\nadb-menu v1.01 - By github.com/julio-gui\n\n");
}

void exibir_dispositivos(void) {
    creditos();
    printf("\nExibindo lista de dispositivos conectados . . .\n");
    
    system("adb devices -l > temp_adb.txt");
    FILE *file = fopen("temp_adb.txt", "r");
    if (file == NULL) {
        printf("Erro ao ler informacoes dos dispositivos.\n");
        return;
    }
    
    char line[256];
    int first_line = 1;
    
    while (fgets(line, sizeof(line), file)) {
        // Pula a primeira linha
        if (first_line) {
            first_line = 0;
            continue;
        }
        
        // Remove quebra de linha
        line[strcspn(line, "\n")] = 0;
        
        // Verifica se é uma linha de dispositivo
        if (strlen(line) > 0 && strstr(line, "device")) {
            char serial[50], product[50], model[50], device[50], transport[50];
            char ip[50] = "N/A"; // Valor padrão caso não consiga obter o IP
            
            // Extrai as informacoes
            sscanf(line, "%s device product:%s model:%s device:%s transport_id:%s", 
                  serial, product, model, device, transport);
            
            // Obtém o endereço IP do dispositivo
            char ip_command[256];
            snprintf(ip_command, sizeof(ip_command), "adb -t %s shell ip addr show wlan0 > temp_ip.txt", transport);
            system(ip_command);
            
            FILE *ip_file = fopen("temp_ip.txt", "r");
            if (ip_file) {
                char ip_line[256];
                while (fgets(ip_line, sizeof(ip_line), ip_file)) {
                    if (strstr(ip_line, "inet ")) {
                        // Extrai o IP da linha (formato: "inet 172.16.96.18/20...")
                        char *ip_start = strstr(ip_line, "inet ") + 5;
                        char *ip_end = strchr(ip_start, '/');
                        if (ip_end) {
                            strncpy(ip, ip_start, ip_end - ip_start);
                            ip[ip_end - ip_start] = '\0';
                        }
                        break;
                    }
                }
                fclose(ip_file);
                remove("temp_ip.txt");
            }
            
            // Exibe organizado
            printf("\nDispositivo %s:\n", device);
            printf("  ID: %s\n", transport);
            printf("  Modelo: %s\n", model);
            printf("  Serial: %s\n", serial);
            printf("  Produto: %s\n", product);
            printf("  Endereco IP: %s\n", ip);
            
            // Verifica se é o dispositivo atual
            if (strcmp(current_device, transport) == 0) {
                printf("  >>> CONECTADO <<<\n");
            }
        }
    }
    printf("\n");
    fclose(file);
    remove("temp_adb.txt");
    pause();
}

void menu_reboot(void) {
    int escolha;
    char id[64];
    
    while (1) {
        clear();
        creditos();
        
        if (strlen(current_device)) {
            printf("Dispositivo atual: %s (%s)\n\n", current_model, current_device);
        }
        
        printf("\n1- Reiniciar\n");
        printf("2- Recovery\n");
        printf("3- Bootloader\n");
        printf("4- Voltar ao menu anterior\n");
        printf("\nEscolha a opcao desejada: ");
        scanf("%d", &escolha);
        getchar();

        if (escolha == 4) break;

        if (strlen(current_device)) {
            strcpy(id, current_device);
        } else {
            clear();
            creditos();
            exibir_dispositivos();
            printf("Digite o ID do dispositivo desejado para prosseguir: ");
            fgets(id, sizeof(id), stdin);
            id[strcspn(id, "\n")] = '\0';
        }

        clear();
        creditos();
        
        switch(escolha) {
            case 1:
                snprintf(string, sizeof(string), "adb -t %s reboot", id);
                system(string);
                printf("Reiniciando o dispositivo conectado . . .\n");
                break;
            case 2:
                snprintf(string, sizeof(string), "adb -t %s reboot recovery", id);
                system(string);
                printf("Reiniciando em modo recovery . . .\n");
                break;
            case 3:
                snprintf(string, sizeof(string), "adb -t %s reboot bootloader", id);
                system(string);
                printf("Reiniciando em modo bootloader . . .\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
        pause();
    }
}

void menu_principal(void) {
    int escolha;
    char nome[64];
    char id[64];
    char ip[64];
    char connect_ip[128];

    while (1) {
        clear();
        creditos();
        
        if (strlen(current_device)) {
            // Menu quando dispositivo está conectado
            printf("Dispositivo conectado: %s (%s)\n\n", current_model, current_device);
            
            printf("0- Sair\n");
            printf("1- Listar dispositivos Android\n");
            printf("2- Conectar a outro dispositivo Android\n");
            printf("3- Desconectar deste dispositivo\n");
            printf("4- Encerrar aplicacao\n");
            printf("5- Opcoes de reinicio\n");
            printf("6- Transferencia de arquivos\n");
            printf("7- Espelhar tela\n");
        } else {
            // Menu quando nenhum dispositivo está conectado
            printf("Nenhum dispositivo conectado\n\n");
            
            printf("0- Sair\n");
            printf("1- Listar dispositivos Android\n");
            printf("2- Conectar a um dispositivo Android\n");
            printf("3- Reiniciar conexao ADB\n");
            printf("4- Conectar a um IP\n");
        }
        
        printf("\nEscolha a opcao desejada: ");
        scanf("%d", &escolha);
        getchar();

        if (strlen(current_device)) {
            // Opções quando dispositivo está conectado
            switch(escolha) {
                case 0:
                    printf("Saindo . . .\n");
                    salvar_configuracao();
                    return;
                case 1:
                    exibir_dispositivos();
                    break;
                case 2:
                    conectar_dispositivo();
                    break;
                case 3:
                    desconectar_dispositivo();
                    break;
                case 4:
                    printf("\nObs.: O dominio pode ser encontrado nas especificacoes do app"
                    "\nExemplo: WhatsApp > com.whatsapp");
                    snprintf(string, sizeof(string), "\n\nInsira o dominio da aplicacao que deseja encerrar em %s: ", current_model);
                    printf(string);
                    fgets(nome, sizeof(nome), stdin);
                    nome[strcspn(nome, "\n")] = '\0';
                    snprintf(string, sizeof(string), "adb -t %s shell am force-stop %s", current_device, nome);
                    system(string);
                    break;
                case 5:
                    menu_reboot();
                    break;
                case 6:
                    menu_transferencias();
                    break;
                case 7:
                    menu_espelhartela();
                    break;
                default:
                    printf("Opcao invalida!\n");
            }
        } else {
            // Opções quando nenhum dispositivo está conectado
            switch(escolha) {
                case 0:
                    printf("Saindo . . .\n");
                    salvar_configuracao();
                    return;
                case 1:
                    exibir_dispositivos();
                    break;
                case 2:
                    conectar_dispositivo();
                    break;
                case 3:
                    system("adb kill-server");
                    system("adb start-server");
                    clear();
                    break;
                case 4:
                    printf("Digite o IP do dispositivo: ");
                    fgets(ip, sizeof(ip), stdin);
                    ip[strcspn(ip, "\n")] = '\0';
                    system("adb tcpip 5555");
                    snprintf(connect_ip, sizeof(connect_ip), "adb connect %s", ip);
                    system(connect_ip);
                    pause();
                    break;
                default:
                    printf("Opcao invalida!\n");
            }
        }
    }
}
