# adb-menu

Um utilitário desenvolvido em C que simplifica o uso do Android Debug Bridge (ADB) e do scrcpy (inicialmente para espelhamento de tela apenas) diretamente pelo terminal, 
permitindo interações rápidas com dispositivos Android conectados via USB ou TCP/IP.

---

## Objetivo

- Facilitar o acesso às funcionalidades do ADB para usuários que não estão familiarizados com comandos de terminal.
- Fornecer uma interface organizada para as operações mais comuns e automatizar processos repetitivos no gerenciamento de dispositivos Android.
- Integração com o **scrcpy** para espelhamento e controle do dispositivo.
- Suporte para conexões USB e TCP/IP.
- Possibilidade de reiniciar, instalar APKs, capturar tela e muito mais.

---

## Funcionalidades Principais

1. Gerenciamento de Dispositivos
- Listar todos os dispositivos conectados com informações detalhadas (modelo, serial, IP)
- Conectar/desconectar dispositivos via USB ou rede
- Salvar automaticamente o último dispositivo conectado

2. Transferência de Arquivos
- Enviar arquivos do PC para o Android
- Receber arquivos do Android para o PC
- Opções para diretórios padrão (Downloads) ou personalizados

3. Controle do Dispositivo
- Reiniciar o dispositivo em diferentes modos (normal, recovery, bootloader)
- Encerrar aplicações específicas
- Espelhar a tela do dispositivo usando scrcpy

## 📦 Dependências

Este programa depende dos seguintes pacotes:

### **ADB (Android Debug Bridge)**
O ADB é uma ferramenta oficial do Android para comunicação entre computador e dispositivos Android.

### **scrcpy (v3.3.1)**

- O scrcpy é um aplicativo que espelha a tela e o áudio de dispositivos Android, permitindo controle via teclado e mouse.  
- Funciona em Linux, Windows e macOS.
- Não requer root e não instala nada no dispositivo.

#### Principais recursos:
- Espelhamento de vídeo e áudio.
- Controle total via teclado/mouse.

Documentação completa do scrcpy:  
[https://github.com/Genymobile/scrcpy/blob/master/README.md](https://github.com/Genymobile/scrcpy/blob/master/README.md)

---

## ⚙️ Compilando o adb-menu.c

Para compilar no Linux:
```bash
gcc .\src\adb-menu.c -o .\bin\adb-menu
```

No Windows (usando MinGW):
```cmd
gcc .\src\adb-menu.c -o .\bin\adb-menu.exe
```

---

## 🚀 Uso

1. Conecte seu dispositivo Android via **USB** ou configure **TCP/IP**.
2. Ative **Depuração USB** nas opções de desenvolvedor do Android.
3. Execute:
```bash
./bin/adb-menu
```

```Windows
.\bin\adb-menu-exe
```

No Windows, execute o programa (normalmente adb-menu.exe).

4. Escolha a opção desejada no menu interativo e siga as instruções específicas de cada função.

---

## Notas Importantes

- O programa mantém um arquivo de configuração (adb_config.cfg) para lembrar o último dispositivo conectado.
- Algumas funções requerem permissões ADB adicionais no dispositivo.
- Para conexão via rede, o dispositivo precisa estar na mesma rede local e ter o modo TCP/IP ativado.

## Dicas

- Use "Reiniciar conexão ADB" se tiver problemas de detecção.
- Para transferência de arquivos, você pode arrastar e soltar arquivos no terminal para preencher o caminho automaticamente (em alguns terminais).
- A função de espelhamento requer o scrcpy instalado na mesma pasta ou no PATH.

---

## 📄 Licença

Este projeto utiliza o **scrcpy** como ferramenta auxiliar.  
O scrcpy é licenciado sob **Apache License, Version 2.0**.

Licença do scrcpy:  
[https://github.com/Genymobile/scrcpy/blob/master/LICENSE](https://github.com/Genymobile/scrcpy/blob/master/LICENSE)

Copyright (C) 2018 Genymobile  
Copyright (C) 2018-2025 Romain Vimont

---

## Créditos

- **ADB** — Ferramenta oficial Google/Android SDK.  
- **scrcpy** — Desenvolvido por **Romain Vimont** e mantido pela **Genymobile**.  
- Este projeto foi desenvolvido por Júlio G. para simplificar o fluxo de trabalho com dispositivos Android, especialmente para desenvolvedores e técnicos que precisam realizar operações ADB com frequência, integrando essas ferramentas para facilitar o uso via menu no terminal. Está aberto a contribuições.
