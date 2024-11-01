# Compilando o APK no Windows com Android Studio

Este guia fornece instruções para compilar o APK do projeto **Grand Line Adventures** usando o Android Studio no Windows.

## Configuração do Ambiente

### 1. Ajuste os caminhos do SDK e NDK

#### Atualizando `local.properties`

Abra o arquivo `local.properties` localizado em: android/com.grandlineadventures.opclient/local.properties

Altere a linha `sdk.dir` para o diretório onde você instalou o SDK do Android. Exemplo: sdk.dir=C:\Caminho\Para\Seu\SDK

### 2. Configuração do arquivo `Android.mk`

O arquivo `Android.mk` está localizado em: android/com.grandlineadventures.opclient/app/jni/src/Android.mk

Todos os arquivos de biblioteca estão incluídos em `/opt/android-ndk`. Para configurar isso corretamente:

1. **Baixe a pasta do NDK** do Docker e coloque-a em uma pasta local no seu disco.
2. Crie uma variável chamada `LOCAL_ANDROID_INCLUDE` apontando para este diretório local. Eexemplo:
  ```
  LOCAL_ANDROID_INCLUDE := C:\\Caminho\\Para\\Seu\\NDK
  ```
3. Adicione $(LOCAL_ANDROID_INCLUDE) antes de todos os caminhos que contêm /opt/android-ndk. Exemplo, em vez de:
  ```
  /opt/android-ndk/external/$(TARGET_ARCH_ABI)/include > $(LOCAL_ANDROID_INCLUDE)/opt/android-ndk/external/$(TARGET_ARCH_ABI)/include
  ```
4. Também é preciso linkar manualmente as libs. Exemplo:
  ```
  -lluajit-5.1 > $(LOCAL_ANDROID_INCLUDE)/opt/android-ndk/external/$(TARGET_ARCH_ABI)/lib/libluajit-5.1.a
  ```
5. Pode ocorrer que o sistema não consiga identificar o libpng, então tente libpng16

6. Inclua os seguintes comandos também para evitar alguns erros de compilação:
  ```
  LOCAL_SHORT_COMMANDS := true
  APP_SHORT_COMMANDS := true
  ```

### 3. Compilação APK
Antes de dar Run você precisa ajustar a paste assets no seu app, então rode esse comando:
```
make makepkg PROJECT=glaclient FULL_APK=true
```
Recomendo, manualmente, no script do makepkg colocar uma forma de ignorar o encrypt dos arquivos também.
