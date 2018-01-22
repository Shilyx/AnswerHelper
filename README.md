# AnswerHelper
##### 各种答题有奖节目（百万英雄、百万赢家、冲顶大会）的辅助工具
* 会自动识别出题目和答案内容
* 自动调用浏览器在百度中搜索问题和答案
* 展示搜索结果并从搜索结果中智能推荐正确选项
* 可有效避免“个性化题目”造成的各类云答题失效
# 使用步骤
## 一、构建nodes组件
### 1. 安装[nodejs最新版本](http://nodejs.cn/download/)
### 2. 命令行进入nodejs\AnswerHelperServer\node_modules\baidu-aip-sdk目录，执行命令：
* `npm install`
### 3. 命令行进入nodejs\AnswerHelperServer目录，执行依次执行命令：
* `npm install mime`
* `npm install cheerio`
* `npm install segment`
### 4. 申请[百度文字识别服务](https://ai.baidu.com/tech/ocr)测试账号，在文字识别服务中选择`创建服务`，获取到`AppID`、`API Key`、`Secret Key`后写入`AnswerHelperServer.js`文件中的相应位置：
```javascript
// 设置APPID/AK/SK
var APP_ID = "申请到的AppID";
var API_KEY = "申请到的API Key";
var SECRET_KEY = "申请到的Secret Key";
```
### 5. 此时js端就配置完了
## 二、构建vc++组件
### 1. 使用vs2008或更高版本打开`AnswerHelper_vc2008.sln`
### 2. 编译win32平台的Release版本的可执行文件`AnswerHelper.exe`
## 三、投入使用
### 1. 放置`AnswerHelper.exe`文件与`AnswerHelperServer`目录在同一目录下
### 2. 在[夜神模拟器](https://www.yeshen.com/)安装答题软件
### 3. 启动`AnswerHelper.exe`程序并将程序区域和出题区域重合
### 4. 题目出现后点击`Do`按钮，程序将自动推荐答案
### 5. 点击`Quit`按钮退出程序
### 6. 中键点击程序界面可以隐藏nodejs的命令行窗口
## 四、其他
### 也可以使用`adb.exe`工具来连接物理手机截图，但是那样比较慢，截图命令：
```bash
adb shell screencap -p /sdcard/screen.png
adb pull /sdcard/screen.png
adb shell rm /sdcard/screen.png
```
### 还可以将手机屏幕投屏到电脑屏幕来提供答题图像。