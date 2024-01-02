#Import("env", "projenv")

import os
import shutil
import glob
import pathlib

def readall(path):
    f = open(path, 'r')
    data = f.read()
    f.close()
    return data


def create_name_from_filename(path):
    filename = os.path.basename(path)
    ret = filename
    ret = ret.replace(".", "_")
    ret = ret.replace(" ", "_")
    return ret


EMBED_DIR=os.path.join(os.getcwd(), "embed")            # embedしたいファイルがおいてあるディレクトリ
OUT_DIR=os.path.join(os.getcwd(), "include", "embed")  # 変換した .h ファイルの出力先
TEMPLATE_FILE=os.path.join(os.getcwd(), "build_script", "template.cpp")

SEP_LENGTH = 90

print("=" * SEP_LENGTH)
print("create_embed.py executing.")
print(f"EMBED_DIR = {EMBED_DIR}")
print(f"OUT_DIR   = {OUT_DIR}")
print("=" * SEP_LENGTH)

#
# Script to build cpp file from embed directory
#
# see: https://thingpulse.com/embed-binary-data-on-esp32/
# see: https://docs.platformio.org/en/latest/projectconf/advanced_scripting.html#before-pre-and-after-post-actions

if os.path.exists(OUT_DIR):
    shutil.rmtree(OUT_DIR)

os.mkdir(OUT_DIR)
template = readall(TEMPLATE_FILE)

for root, dirs, files in os.walk(EMBED_DIR):

    for filename in files:
        print(root, filename)
        file = os.path.join(root, filename)

        if file.endswith(".md") or file.endswith(".sh"):
            print(f"ignored markdown file {file.name}")
            continue

        # start generating source
        output = template
        data = readall(file)

        output_name = create_name_from_filename(file)

        # 変数名部分を置換。 変数名は全部大文字とする
        output = output.replace("$$REPLACE_NAME$$", output_name.upper()).replace("$$REPLACE_CONTENT$$", data)
        output_cpp_path = os.path.join(OUT_DIR, output_name + ".h")

        # todo ファイル名重複チェック。すでにあったらコケる

        f = open(output_cpp_path, 'w')
        data = f.write(output)
        f.close()

        print(f"generated {output_cpp_path}")


print("=" * SEP_LENGTH)
print("DONE")
print("=" * SEP_LENGTH)
