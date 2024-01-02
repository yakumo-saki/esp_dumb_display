"use strict";

const DEBUG_MODE = false;
const DEBUG_API_HOST = "10.1.0.110"; // デバッグ時、APIを投げる先

const CONTENT_JSON = "application/json";
const CONTENT_TEXT = "text/plain";
const CONTENT_FORM = "application/x-www-form-urlencoded"
const IGNORE_KEY = ["settingId"]

let SETUP_MODE = false;

// 画面ロード時のconfig。保存時に差分を取るために使用する。
let LAST_CONFIG = null;

function toPath(relativePath) {
    if (!DEBUG_MODE) {
        return relativePath;
    }

    var path = "http://" + DEBUG_API_HOST;
    if (!relativePath.startsWith("/")) {
        path = path + "/";
    }

    return path + relativePath;
}

async function httpGet(relativePath, contentType = CONTENT_JSON) {
    if (DEBUG_MODE) {
        console.log("httpGet " + toPath(relativePath));
    }

    const ret = await fetch(toPath(relativePath), {
        method: "GET",
        cache: "no-cache",
        headers: {
            "Content-Type": contentType,
        },
    });

    if (!ret.ok) {
        throw `httpGet: ${relativePath} response is not ok. ${ret.status} ${ret.statusText}`
    }

    return ret;
}

/**
 * 
 * @param {string} relativePath 
 * @param {object} body 
 * @param {string} contentType CONTENT_*
 * @returns promise
 */
async function httpPost(relativePath, body, contentType = CONTENT_TEXT) {
    if (DEBUG_MODE) {
        console.log("httpPost " + toPath(relativePath));
    }

    const ret = await fetch(toPath(relativePath), {
        method: "POST",
        cache: "no-cache",
        headers: {
            "Content-Type": contentType,
        },
        body: body
    });

    if (!ret.ok) {
        throw `httpPost: ${relativePath} response is not ok. ${ret.status} ${ret.statusText}`
    }

    return ret;
}

function replaceVersion(replaceName, value) {
    const elems = document.querySelectorAll(`span[replace='${replaceName}']`);
    elems.forEach((element) => {
        element.innerHTML = value;
    });
}

/**
 * ページロード時にEBXのバージョン等を置き換える
 */
async function setPageValues() {
    const res = await httpGet("/ping");
    console.log(res);

    const json = await res.json();
    console.log(json);

    SETUP_MODE = json["mode"] == "SETUP";

    replaceVersion("productVer", `ver.${json["majorVer"]}.${json["minorVer"]}`);
    replaceVersion("settingId", json["settingId"]);
}

async function showSetupModeOnly() {
    const elements = document.querySelectorAll(".setupModeOnly");
    elements.forEach(element => {
        element.hidden = !SETUP_MODE;
    });

}

/**
 * configMap の値をドキュメントにセットする
 */
function setConfigValuesToPage(configMap) {
    configMap.forEach((v, k) => {

        // console.log("key", k, "val", v);

        const el = document.querySelector(`input[name="${k}"]`);
        if (el == null) {
            console.log(`key ${k} is not found on document.`);
            return;
        }

        if (el.type === "radio") {
            const radio = document.querySelector(
                `input[name="${k}"][value="${v}"]`
            );
            if (radio == null) {
                console.log(
                    `radio button name=${k} value=${v} is not found on document.`
                );
            }
            radio.checked = true;
        } else {
            // input type=text,password,number
            el.value = v;
        }
    });
}

/**
 * configのキー、値をWeb上の要素にセット。checkboxには対応していない
 * @param {string} key 
 * @returns boolean 成功 or 失敗
 */
function setInputValue(key, value) {

    const els = document.querySelectorAll(`input[name='${key}']`);
    if (els.length > 1) {
        // RADIO
        const elements = document.querySelectorAll(`input[name='${key}'][value='${value}']`);
        if (elements.length > 1) {
            console.log(`failed. multiple elements found ${key}=${value}`);
            return false;
        } else if (elements.length < 1) {
            console.log(`failed. no elements found ${key}=${value}`);
            return false;
        }

        elements[0].checked = true;
        return;
    } else if (els.length == 1) {
        els[0].value = value;
        return true;
    } else if (els.length == 0) {
        console.log(key, "element not found");
        return false;
    }

    return true;
}


/**
 * Web上の要素から値を取得
 * @param {string} key 
 * @returns string
 */
function getInputValue(key) {

    const els = document.querySelectorAll(`input[name='${key}']`);
    if (els.length > 1) {
        // RADIO
        const el = document.querySelector(`input[name='${key}']:checked`);
        return el.value;
    } else if (els.length == 1) {
        return els[0].value;
    } else if (els.length == 0) {
        console.log(key, "element not found");
        return null;
    }

    return "INVALID";
}

/**
 * org -> new で異なっているものだけをMapにして返す
 * @param {Map<string,string>} orgMap 
 * @param {Map<string,string>} newMap
 * @return {Map<string,string>}
 */
function createConfigDiff(orgMap, newMap) {
    const ret = new Map();

    orgMap.forEach((val, key) => {
        const newVal = newMap.get(key);
        // console.log("diff", key, val, newVal);
        if (val !== newVal) {
            ret.set(key, newVal);
        }
    });

    return ret;
}

/**
 * ConfigをEnvBoyに保存するAPIを呼ぶ
 * @param {Map<string,string>} configMap 
 * @return {object} result
 */
async function saveConfig(configMap) {
    const params = new URLSearchParams()
    configMap.forEach((v,k) => params.append(k, v));

    try {
        const res = await httpPost("/api/v1/config", params, CONTENT_FORM);
        return res;
    } catch (err) {
        throw `Failed to send config: ${err}`;
    }
}

async function commitRevertConfig(isCommit = true) {
    const URL = "/api/v1/config/" + (isCommit ? "commit" : "revert");
    try {
        const commit = await httpPost(URL, "", CONTENT_JSON);
        return commit;
    } catch (err) {
        throw `Failed to commit config: ${err}`;
    }
}

async function commitConfig() {
    return commitRevertConfig(true);
}

async function revertConfig() {
    return commitRevertConfig(false);
}


/**
 * 保存ボタンの処理
 */
async function saveConfigButton() {

    const waitDialog = document.getElementById("waitDialog");

    const newConfig = new Map();

    // INPUTから取得
    LAST_CONFIG.forEach((_, key) => {
        const val = getInputValue(key);
        newConfig.set(key, val);
    });

    console.log(newConfig);

    // 差分作成
    let configDiff = createConfigDiff(LAST_CONFIG, newConfig);

    let confirmMessage = "変更を保存してよろしいですか？";
    if (configDiff.size == 0) {
        confirmMessage = confirmMessage + "\n\n※設定内容に変更がありません";
    } 
    
    if (!window.confirm(confirmMessage)) {
        return;
    }

    try {
        waitDialog.showModal();

        let needReboot = false;
        if (configDiff.size > 0) {
            const res = await saveConfig(configDiff);
            const result = await res.json();
            if (result.success != true) {
                alert("設定内容にエラーがあります。設定は反映されていません。\n" + result.message + "\n"
                        + result.msgs.join("\n"));
                return;
            }
            needReboot = result.needReboot;
        }

        const commit = await commitConfig();
        const commitResult = await commit.json();
        if (commitResult.success != true) {
            alert("設定の保存に失敗しました。");
            return;
        }

        if (needReboot) {
            alert("設定を保存しました。\n再起動するまで反映されない設定があります。");
        } else {
            alert("設定を保存しました。");
        }

    } catch (err) {
        console.log(err);
        return;
    } finally {
        waitDialog.close();
    }

}

/**
 * configを取得してきたObjectからMapに変換する。
 * ついでに不要な項目は捨てる
 * @param {Object} configObj
 * @return {Map<string,string>}
 */
function configObjectToMap(configObj) {
    const map = new Map();
    Object.keys(configObj).forEach(k => {
        if (IGNORE_KEY.includes(k)) {
            return;
        }
        map.set(k, configObj[k]);
    });
    return map;
}

async function loadConfig() {
    try {
        const res = await httpGet("/api/v1/config", CONTENT_JSON);
        const json = await res.json();
        console.log(json);

        if (!json["success"]) {
            alert("エラーが発生しました。");
            return;
        }

        const configObj = json["config"];
        const confMap = configObjectToMap(configObj);
        
        confMap.set("password", ""); // Add password as empty for later use
        LAST_CONFIG = confMap;
    
        setConfigValuesToPage(confMap);
    
    } catch (err) {
        alert(err);
        return;
    }

}

document.addEventListener("DOMContentLoaded", async (event) => {
    const waitDialog = document.getElementById("waitDialog");
    waitDialog.showModal();

    console.log("DOM fully loaded and parsed");
    document.getElementById("submit").addEventListener("click", async (event) => {
        console.log("Submit Clicked");
        await saveConfigButton();
    });

    await setPageValues();
    await showSetupModeOnly();
    await loadConfig();

    waitDialog.close();
});

