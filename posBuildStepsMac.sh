TYPE='debug'
#install_name_tool -id ~/Dropbox/workspace/bigCellBrother/Maclib/libbigCellBrother.dylib ~/Dropbox/workspace/bigCellBrother/Maclib/libbigCellBrother.dylib
#install_name_tool -change libbigCellBrother.dylib ~/Dropbox/workspace/bigCellBrother/Maclib/libbigCellBrother.dylib ~/Dropbox/workspace/build/bcb_mac_$TYPE/BigCellBrothergui.app/Contents/MacOS/BigCellBrothergui
dylibbundler -od -b -x ~/Dropbox/workspace/build/bcb_mac_$TYPE/BigCellBrothergui.app/Contents/MacOS/BigCellBrothergui -cd -d ~/Dropbox/workspace/build/bcb_mac_$TYPE/BigCellBrothergui.app/Contents/libs/
macdeployqt ~/Dropbox/workspace/build/bcb_mac_$TYPE/BigCellBrothergui.app/
