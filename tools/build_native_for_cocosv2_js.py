#!/usr/bin/python
# build_native.py
# Build native codes

import sys
import os, os.path
import shutil
from optparse import OptionParser

def check_environment_variables():
    ''' Checking the environment NDK_ROOT, which will be used for building
    '''

    try:
        NDK_ROOT = os.environ['NDK_ROOT']
    except Exception:
        print "NDK_ROOT not defined. Please define NDK_ROOT in your environment"
        sys.exit(1)

    return NDK_ROOT

def select_toolchain_version():
    '''
    Conclution:
    ndk-r9d   -> use gcc4.8
    '''

    ndk_root = check_environment_variables()
    if os.path.isdir(os.path.join(ndk_root,"toolchains/arm-linux-androideabi-4.8")):
        os.environ['NDK_TOOLCHAIN_VERSION'] = '4.8'
        print "The Selected NDK toolchain version was 4.8 !"
    elif os.path.isdir(os.path.join(ndk_root,"toolchains/arm-linux-androideabi-4.7")):
        os.environ['NDK_TOOLCHAIN_VERSION'] = '4.7'
        print "The Selected NDK toolchain version was 4.7 !"
    else:
        print "Couldn't find the gcc toolchain."
        exit(1)

def do_build(cocos_root, ndk_root, app_android_root, build_mode):

    ndk_path = os.path.join(ndk_root, "ndk-build")

    # windows should use ";" to seperate module paths
    platform = sys.platform
    if platform == 'win32':
        ndk_module_path = 'NDK_MODULE_PATH=%s;%s/cocos2dx/platform/third_party/android/prebuilt;%s' % (cocos_root, cocos_root, app_android_root)
    else:
        ndk_module_path = 'NDK_MODULE_PATH=%s:%s/cocos2dx/platform/third_party/android/prebuilt:%s' % (cocos_root, cocos_root, app_android_root)

    ndk_build_param = sys.argv[1:]
    if len(ndk_build_param) == 0:
        command = '%s -C %s NDK_DEBUG=%d %s' % (ndk_path, app_android_root, build_mode, ndk_module_path)
    else:
        command = '%s -C %s NDK_DEBUG=%d %s %s' % (ndk_path, app_android_root, build_mode, ''.join(str(e) for e in ndk_build_param), ndk_module_path)
    os.system(command)

def copy_files(src, dst):

    for item in os.listdir(src):
        path = os.path.join(src, item)
        # Android can not package the file that ends with ".gz"
        if not item.startswith('.') and not item.endswith('.gz') and os.path.isfile(path):
            shutil.copy(path, dst)
        if os.path.isdir(path):
            new_dst = os.path.join(dst, item)
            os.mkdir(new_dst)
            copy_files(path, new_dst)

def copy_resources(app_android_root):

    # remove app_android_root/assets if it exists
    assets_dir = os.path.join(app_android_root, "assets")
    if os.path.isdir(assets_dir):
        shutil.rmtree(assets_dir)

    # copy resources
    os.mkdir(assets_dir)
    resources_dir = os.path.join(app_android_root, "../Resources")
    if os.path.isdir(resources_dir):
        copy_files(resources_dir, assets_dir)

    script_dir = os.path.join(app_android_root, "../../../scripting/javascript/bindings/js")
    if os.path.isdir(script_dir):
        copy_files(script_dir, assets_dir)

def build(build_mode):

    if build_mode is None:
        build_mode = 0
    elif build_mode == 'debug':
        build_mode = 1
    else:
        build_mode = 0

    ndk_root = check_environment_variables()
    select_toolchain_version()

    current_dir = os.path.dirname(os.path.realpath(__file__))
    cocos_root = os.path.join(current_dir, "../../..")

    app_android_root = current_dir
    copy_resources(app_android_root)
    do_build(cocos_root, ndk_root, app_android_root, build_mode)

# -------------- main --------------
if __name__ == '__main__':

    usage = """
    Usage: python build_native.py -b debug
    """

    parser = OptionParser(usage=usage)
    parser.add_option("-b", "--build", dest="build_mode",
    help='The build mode for native project,debug or release[default].')
    (opts, args) = parser.parse_args()

    build(opts.build_mode)
