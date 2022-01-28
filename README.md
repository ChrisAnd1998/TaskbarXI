# TaskbarXI
An application written in C++ to modify the Windows 11 Taskbar. Current version turns the Windows 11 Taskbar into a dock. New features will be added in the future.  
![TaskbarXI](https://user-images.githubusercontent.com/50437199/148023157-7d2c8a9d-cd82-499c-9a98-52d53625296a.png)




## **Features (Release 1.0.3)**
* Turn the Windows 11 Taskbar into a dock.  
* Turn the tray/clock into a dock.  
* Support multiple monitors.  
* Support sepperate DPI scaling. 
* Switch back to normal on maximized window.  
* Support left and centered taskbar.
* Tray icon to exit TaskbarXI and revert to the default taskbar.
* Simple GUI to change the settings.  

&nbsp;

## **Upcoming Features**
* Change taskbar background style and color.  

&nbsp;

## **Parameters**
**With 1.0.3 release all Parameters can be changed with GUI**,<br/><br/> 
If you still want to change properties with console, here are Parameters:
* **`-stop`** parameter (Stops TaskbarXI and reverts the taskbar to default)
* **`-restart`** parameter (Does not refresh the taskbar region when starting)
* **`-square`** parameter (Uses square corners instead of rounded corners)
* **`-radius <radius>`** parameter (Define the corner radius you want to be used)
* **`-ignoremax`** parameter (Does not revert the taskbar on maximized window)
* **`-notray`** parameter (Disables system tray icon)
* **`-hidetraywnd`** parameter (Hides the system tray area)
* **`-createstartup`** parameter (Creates a startup entry including the current parameters)
* **`-removestartup`** parameter (Removes startup entry and exits TaskbarXI)
* **`-console`** parameter (Displays a console window)
* **`-sticky`** parameter (Sticks the system tray to the taskbar (removes the tray icons to keep it stable))
* **`-help`** parameter (Displays a help window)
* **`-smoothresize`** parameter (Resizes the taskbar smoothly)
* **`-expandspeed <speed>`** parameter (Define the speed you want to be used for the expand animation (default: 90))
* **`-shrinkspeed <speed>`** parameter (Define the speed you want to be used for the shrink animation (default: 700))
* **`-blur`** parameter (Makes the taskbar blurred) <-- **EXPERIMENTAL**



  
**CMD Example:** `"TaskbarXI.exe" -ignoremax -expandspeed 100 -square`  



&nbsp;

**Thanks to: ❤️**  
[@PrincessAkira](https://github.com/PrincessAkira)  
[@dmitryaleshin](https://github.com/dmitryaleshin)  
[@officialLennox](https://github.com/officialLennox)  

&nbsp;

**Screenshots**
<img width="1920" alt="Untitled" src="https://user-images.githubusercontent.com/50437199/147857018-9de30327-9f5c-48c6-ab52-9ed7afefb070.png">
<img width="1920" alt="Untitled" src="https://user-images.githubusercontent.com/50437199/147869295-0736389b-95cd-4875-9ab0-cdce03df22b7.jpg">
<img width="1920" alt="Untitled" src="https://user-images.githubusercontent.com/50437199/147869297-a07983cd-02cc-4d28-a40b-066064d2f210.jpg">
