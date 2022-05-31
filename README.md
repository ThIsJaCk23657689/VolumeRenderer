# Volume Renderer
新版的 Volume Renderer，使用 C++ 和 OpenGL 開發，搭配 SDL2 以及 ImGui。 

> 傳統解釋檔案格式的資訊檔為 `.inf`，但規範沒有統一十分不建議，目前統一使用 `.toml` 格式做為新版的資訊檔。

![](https://i.imgur.com/toAD1Du.png)
![](https://i.imgur.com/9ffjtuu.png)
![](https://i.imgur.com/XvjUEQZ.png)

## 安裝需求
需要使用 vcpkg 安裝以下套件：
* sdl2 - 2.0.22
* glad - 0.1.34#3
* glm - 0.9.9.8#1
* imgui[core,opengl3-binding,sdl2-binding] - 1.87
* stb - 2021-09-10
* tomlplusplus 3.0.1

> 圖學環境安裝教學請[看這](https://hackmd.io/@23657689/computer_graphics_env_settings) 。

## 操作
* `滑鼠` 旋轉 Camera 觀看視角
* `TAB` 切換滑鼠控制攝影機（預設是關閉的），開啟時鼠標會不見且可以控制攝影機
* `x` 隱藏或顯示三軸 (世界坐標系原點)
* `Ctrl` + `q` 關閉程式
* `F11` 切換全螢幕 / 視窗

## 備註
1. 如果使用 Mingw 編譯的話，請記得 vcpkg 的套件要安裝 `x64-mingw-dynamic` 的版本，以及 CMake 需要新增 `-DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic` 以及 shader file 的換行符號要改為 `LF` 才不會發生編譯錯誤。

## Future Works
1. 可以嘗試使用 OpenMP 平行化一些計算
2. 利用 Threads 多執行緒讓讀檔和處理 volume data 時依舊可以保持 application 運行順暢
3. 產生用於預覽的 transfer function colormap 縮圖
4. 將 cubic bezier 功能加入到 transfer function，除了線性內插之外還有其他模式。

------------------------------------------------------------
國立臺灣海洋大學 資訊工程學系 電腦圖學實驗室 Lab 503

**課程名稱**：電腦圖學

**授課教授**：翁世光 教授

**課程助教**：張竣傑

**助教信箱**：10957037@email.ntou.edu.tw