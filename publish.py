#!/usr/bin/python
# build_native.py

import sys
import os, os.path
import shutil
from optparse import OptionParser

def copy_files(src, dst):

    current_dir = os.path.dirname(os.path.realpath(__file__))
    srd_dir = os.path.join(current_dir, src)
    dst_dir = os.path.join(current_dir, dst)

    if os.path.isdir(srd_dir):
        if os.path.isdir(dst_dir):
            shutil.rmtree(dst_dir)
        os.mkdir(dst_dir)

    if os.path.isdir(srd_dir):
        for item in os.listdir(srd_dir):
            path = os.path.join(srd_dir, item)
            if not item.startswith('.') and os.path.isfile(path):
                shutil.copy(path, dst_dir)
            if os.path.isdir(path):
                new_dst = os.path.join(dst_dir, item)
                os.mkdir(new_dst)
                copy_files(path, new_dst)
    else:
        shutil.copy(srd_dir, dst_dir)

# -------------- main --------------
if __name__ == '__main__':

    parser = OptionParser()

    copy_files("common/gplay.h", "gplay_for_cpp/gplay.h")
    copy_files("common/gplay.h", "gplay_for_js/gplay.h")
    copy_files("common/gplay.h", "gplay_for_lua/gplay.h")

    copy_files("common/cocos_bridge", "gplay_for_cpp/cocos_bridge")
    copy_files("common/cocos_bridge", "gplay_for_js/cocos_bridge")
    copy_files("common/cocos_bridge", "gplay_for_lua/cocos_bridge")

    copy_files("common/prebuilt", "gplay_for_cpp/prebuilt")
    copy_files("common/prebuilt", "gplay_for_js/prebuilt")
    copy_files("common/prebuilt", "gplay_for_lua/prebuilt")
