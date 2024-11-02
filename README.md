# Compiling glaclient on Android Studio

## Environment configuration

### 1. Adjust SDK path

Open the file `android/com.grandlineadventures.opclient/local.properties`.

Change `sdk.dir` to the folder where the Android SDK is installed. Example: ```sdk.dir=E:\\Android```
### 2. Configuring `Android.mk`

## 1. **Downaloding dependencies from Docker**.
1. In the WSL context in the `luna/scripts/compile` folder, run the command `make docker-term-android PROJECT=glacient`.
2. In the Linux context run the command: ```cd /opt/android-ndk/ && tar -cvf external.tar.gz ./external```
3. After that, in another WSL context, run the ```docker ps``` command, get the CONTAINER ID of the image where you ran the last command and run this command ```docker cp <container_id>:/opt/android-ndk/ /mnt/c/your/dest/here```
4. Now you can close the container and extract the copied files.
## 2. **Finally, configuring the file**
1. Open the file `android/com.grandlineadventures.opclient/app/jni/src/Android.mk`.
2. Create a variable to specify the path to the directory where you extract the ndk files. Example: ```LOCAL_ANDROID_INCLUDE := C:/your/dest/here```
4. Add the variable before all places using /opt/android-ndk. Example: ```$(LOCAL_ANDROID_INCLUDE)/opt/android-ndk/external/$(TARGET_ARCH_ABI)/include```
5. You'll also need link the ndk libraries with their full paths. ```-lluajit-5.1``` > ```$(LOCAL_ANDROID_INCLUDE)/opt/android-ndk/external/$(TARGET_ARCH_ABI)/lib/libluajit-5.1.a```
6. Include the following command in the file ```LOCAL_SHORT_COMMANDS := true``` and ```APP_SHORT_COMMANDS := true```

### 3. Compiling APK
Before compile you'll need run this command in `luna/scripts/compile` ```make makepkg PROJECT=glaclient FULL_APK=true```

Don't forget to set to ignore encrypt in makepkg.lua
