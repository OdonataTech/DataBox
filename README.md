# DataBox
The DataBox is a free FileMaker plugin that allows creating and manipulating of data in computer memory. You can store data like variables, arrays, tables or any hierarchical structures and manipulate them.

As opposed to FileMaker global variables (like $$name) the data is stored in memory in correct FileMaker format like number, text, date, time or timestamp.

As the DataBox stores all data in memory, it works very fast. You can get data from your database (ex. from ExecuteSQL functions ) once and then work with it in memory.

The DataBox has powerful functions for manipulating, aggregating and getting data in different formats like JSON or interactive html-table.

See details and download binary files https://tria.pw/databox/index.html

## Notes for compile 
These sources contain only the Xcode project file for the macOS version. For compilation, you also need to download FileMaker SDK. It is available on this page https://www.claris.com/resources/downloads/

The files structure with the Filemaker plugin frameworks file can look like this:

    DataBoxPlugin
        DataBox
            Headers
                DataBox.h
                ....
            Source
                DB_MainFuncs.cpp
                ...
            DataBox.xcodeproj
            Info.plist
        Headers
            FMWrapper
                FMXBinaryData.h
                ....
        Libraries
            Mac
            Win
            ...
        
If you want to make a version for Windows you can use MiniExample from FileMaker PlugInSDK. MiniExample contains needed project files for macOS and for Visual Studio. MiniExample contains needed project files for macOS and for Visual Studio. Deploy MiniExample on your Windows machine, delete FMMiniPlugIn.cpp and add the Databox's .h and .cpp files to the project.

