/**
 * Created by lt on 2018/1/13.
 */

const global_localhost = "127.18.1.13";
const global_port = parseInt(process.argv[2] || "12345") || 12345;
const AipOcrClient = require("baidu-aip-sdk").ocr;
const Segment = require('segment');
const POSTAG = require("./POSTAG");

// 加载词库
const segment = new Segment();

segment.useDefault();

var fs = require('fs');

// 设置APPID/AK/SK
var APP_ID = "";
var API_KEY = "";
var SECRET_KEY = "";

// 新建一个ocr服务对象，建议只保存一个对象调用服务接口
var client = new AipOcrClient(APP_ID, API_KEY, SECRET_KEY);

// ocr可选参数
var options = {};
options["language_type"] = "CHN_ENG";
options["detect_direction"] = "true";
options["detect_language"] = "true";
options["probability"] = "false";

// 当前关键词列表
var global_keywords = ["阿里巴巴的ceo是谁", "你", "我", "马云"];

// 测试
function test() {
    var data = [
        // [ '中国古代砸缸救小伙伴的人是?', '司马迁', '司马光', '司马蕗' ] ,
        // [ '一杯糖水喝掉一半后,糖水的密度变化是?', '变大', '变小', '不变' ] ,
        // [ '美美国家庭在感恩节基本都会吃的传统食物是?', '火鸡', '烤鸭', '烤乳猪' ] ,
        // [ '“范进中举”的故事节选自下列哪本书籍?', '《聊斋》', '《儒林外史》', '《菜根谭》' ] ,
        // [ '以下哪项不是电脑的输出设备?', '显示器', '鼠标', '打印机' ] ,
        // [ '充入食品包装袋,可以起到防腐作用的气体是?', '氧气', '二氧化碳', '氮气' ] ,
        // [ '清朝顺治皇帝的生母是?', '甄嬛', '纯元皇后', '孝庄文皇后' ] ,
        // [ '在周杰伦的歌曲中,下列哪一首不是由方文山作词?', '《青花瓷', '《菊花台》', '《 晴天' ] ,
        // [ '下列哪一个皇帝自创了一种书法字体?', '宋徽宗', '唐太宗', '明成祖' ] ,
        // [ '以下哪个选项的计算结果为1?', '1与1的倒数相加', '1与1的相反数相乘', '1与1的绝对值相乘' ] ,
        // [ '我们常说"闯关东”,其中“关"是指哪一关?', '玉门关', '函谷关', '山海关' ] ,
        // [ '“法棍”是什么东西?', '法式面包', '绝缘棒', '一种冷兵器' ] ,
        // [ '我国法定结婚年岭是多大?', '男22岁女20岁', '男24岁女22岁', '男25岁女23岁' ] ,
        // [ '下列哪种做法或设计是为了增大摩擦力?', '旅行箱下的小轮子', '轮胎上的花纹', '给齿轮加润滑油' ] ,
        // [ '世界闻名的电影工业中心好莱坞位于哪个国家?', '法国', '美国', '英国' ] ,
        // [ '中药”阿胶”的主要原料是?', '驴皮', '羊皮', '鹿皮' ] ,
        // [ '下列哪种名茶的主要产地在云南?', '铁观音', '君山银针', '普洱' ] ,
        // [ '家用电线中的导体一般采用什么材料?', '铁', '铜', '铅' ] ,
        // [ '“黍离之悲"用用来形容国破家亡之痛,这个成语出自以', '《尚书》', '《诗经》', '《礼记》' ] ,
        // [ '将适量的水、苯、水银倒入足够大的烧杯中,哪种物', '水', '苯', '水银' ]
    ];

    data.forEach((line) => {
        global_keywords = line;
        guessAndReport();
    });
}
// test(); return;

// 猜测答案并汇报
function guessAndReport() {
    var https = require("http");
    var cheerio = require("cheerio");
    var question = "http://www.baidu.com/s?rn=25&wd=" + encodeURIComponent(global_keywords[0]);
    var chunks = [];
    var length = 0;

    var isPWanted = function (w, p) {
        if (!p) {
            p = 0;
        }
        var wanted =
            p == POSTAG.D_I ||
            p == POSTAG.D_A ||
            p == POSTAG.D_F ||
            p == POSTAG.D_L ||
            p == POSTAG.A_M ||
            p == POSTAG.D_MQ ||
            p == POSTAG.D_N ||
            p == POSTAG.D_S ||
            p == POSTAG.D_T ||
            p == POSTAG.D_V ||
            p == POSTAG.A_NR ||
            p == POSTAG.A_NS ||
            p == POSTAG.A_NT ||
            p == POSTAG.A_NX ||
            p == POSTAG.A_NZ ||
            p == POSTAG.UNK;
        console.log("### ", wanted ? "保留" : "忽略", " ### ", w, " ### ", POSTAG.chsName(p), " ### 0x" + (p || 0).toString(16));
        return wanted;
    };

    var getWordCount = (text, word) => {
        if (!word) {
            return 0;
        }

        var adjust_word = "";
        for (var i = 0; i < word.length; ++i) {
            if ("`~$^&*-+.><[]{}()|\\/".indexOf(word[i]) < 0) {
                adjust_word += word[i];
            }
        }

        if (adjust_word.length == 0) {
            return 0;
        }

        var seg_result = segment.doSegment(adjust_word, {
            convertSynonym: true,
            stripPunctuation: true
        });
        
        var result = (text.match(new RegExp(adjust_word, "gi")) || []).length * 10;
        for (var i = 0; i < seg_result.length; ++i) {
            if (isPWanted(seg_result[i].w, seg_result[i].p)) {
                result += (text.match(new RegExp(seg_result[i].w, "gi")) || []).length;
            }
        }

        return result;
    };

    var guessInWebPage = function (web_text) {
        if (global_keywords[0] == "null" || global_keywords[1] == "null" || global_keywords[2] == "null" || global_keywords[0] == "null") {
            return "题目不全";
        }

        var temp_obj = {
            "A": getWordCount(web_text, global_keywords[1]),
            "B": getWordCount(web_text, global_keywords[2]),
            "C": getWordCount(web_text, global_keywords[3])
        };

        if (temp_obj["A"] == 0 && temp_obj["B"] == 0 && temp_obj["C"] == 0) {
            return "无法判断";
        }

        var result = "";
        var reverse = global_keywords[0].indexOf("没") >= 0 ||
            global_keywords[0].indexOf("不") >= 0 ||
            global_keywords[0].indexOf("未") >= 0;
        var less = "X";
        var less_count = 1000000;
        var more = "X";
        var more_count = 0;

        if (reverse) {
            result += "【非】";
        }

        Object.keys(temp_obj).forEach((key) => {
            result += (key + "(" + temp_obj[key] + ") ");
            if (temp_obj[key] > more_count) {
                more = key;
                more_count = temp_obj[key];
            }
            if (temp_obj[key] < less_count) {
                less = key;
                less_count = temp_obj[key];
            }
        });

        result += (`大概是【 ${reverse ? less : more} 】`);
        return result;
    };

    var report = function (result) {
        const client = require('dgram').createSocket('udp4');
        client.send(Buffer.from(result), global_port + 1, global_localhost, (err) => {
            client.close();
        });
    }

    https.get(question, (res) => {
        res.on("data", (data) => {
            chunks.push(data);
            length += data.length;
        });

        res.on("end", () => {
            try {
                var chunk = Buffer.concat(chunks, length);
                var $ = cheerio.load(chunk);
                var text = $("#content_left").text();
                var result = guessInWebPage(text);

                console.log(result);
                report(result);
            } catch (err) {
                console.log(err);
            }
        });

        res.on("error", (err) => {
            console.log(err);
        });
    }).on("error", (err) => {
        console.log(err);
    });;
}

// 在线处理ocr
function onGetScreenshot(pic_path) {
    console.log("---------------------------------------------");
    client.generalBasic(fs.readFileSync(pic_path).toString("base64"), options).then((result) => {
        // { log_id: 2443414709187805700,
        //   direction: 0,
        //   words_result_num: 4,
        //   words_result:
        //    [ { words: '2.出生在2月13日,是什么星座?' },
        //      { words: '摩羯座' },
        //      { words: '水瓶座' },
        //      { words: '双鱼座' } ],
        //   language: -1 }
        if (result && result.words_result) {
            global_keywords = [];
            result.words_result.forEach((obj) => {
                if (obj && obj.words) {
                    global_keywords.push(obj.words.replace(/^\d\d?\./, ""));
                }
            });
            while (global_keywords.length > 4) {
                global_keywords[1] = global_keywords[0] + global_keywords[1];
                global_keywords.shift();
            }
            while (global_keywords.length < 4) {
                global_keywords.push("null");
            }
            guessAndReport();
            console.log(global_keywords);
        }
    }).catch((err) => {
        console.log(err);
    });
}

// udp 服务器
var dgram = require("dgram");
const udp_sock = dgram.createSocket('udp4');

udp_sock.bind(global_port, global_localhost);

udp_sock.on("message", (text) => {
    onGetScreenshot(text.toString());
});

udp_sock.on('listening', () => {
    const address = udp_sock.address();
    console.log(`udp服务器监听 ${address.address}:${address.port}`);
});

udp_sock.on('error', (err) => {
    console.log(`udp服务器异常：\n${err.stack}`);
    udp_sock.close();
});

// http服务器
const http = require("http");
const cp = require("child_process");
const url = require("url");
const path = require('path');
const mime = require("mime");
const querystring = require('querystring');

const server = http.createServer((req, res) => {
    var urlParsed = url.parse(req.url);

    if (!urlParsed.pathname || urlParsed.pathname.length == 0 || urlParsed.pathname == "/") {
        urlParsed.pathname = "/index.html";
    }

    var finalPath = path.join(__dirname, "www", urlParsed.pathname);

    if (urlParsed.pathname.indexOf("..") >= 0) {
        res.writeHead(500, { 'Content-Type': 'text/html' });
        res.end("<h1>attack</h1>");
        return;
    }

    if (urlParsed.pathname == "/global_keywords.action") {
        var querys = querystring.parse(urlParsed.query);
        res.writeHead(200, { 'Content-Type': 'application/javascript' });
        res.end(`${querys.callback}(${JSON.stringify(global_keywords)});`);
        return;
    }

    var contentType = mime.getType(finalPath) || 'text/html';

    fs.readFile(finalPath, (err, data) => {
        if (err) {
            res.writeHead(500, { 'Content-Type': 'text/html' });
            res.end(`<h1>${err.stack}</h1>`);
        } else {
            res.writeHead(200, { 'Content-Type': contentType });
            res.end(data);
        }
    });
});

server.listen(global_port, global_localhost);

server.on('listening', () => {
    const address = server.address();
    console.log(`http服务器监听 ${address.address}:${address.port}`);
    cp.exec(`start http://${address.address}:${address.port}`);
});