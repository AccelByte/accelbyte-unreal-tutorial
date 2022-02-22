# Unreal Tutorial Project

This project will help you to integrate the sdk into your current project.

## Current features implemented :

- Login
- MainMenu
- Lobby
- Friends
- Party
- Matchmaking
- Dedicated Server Integration
- Gameplay Integration

## Setup Project

Preparation Setup:

1. Go to `TutorialProject.uproject` by accessing `FolderCloneDestination/TutorialProject`
and right click at `TutorialProject.uproject`, select `Generate Visual Studio project files`.

2. After the generating Visual Studio done, there will be `TutorialProject.sln` ready to be opened.
You can also opening Solution with other IDE, but we recommend you using Visual Studio 2019 since it's already
tested and works well.

3. After the project opened, Find in the Solution Explorer a folder called `Games`, open the dropdown
and you will find `TutorialProject.vcxproj` , right click on it and choose `Set as Startup Project`.

4. After it's done we have 2 ways, `Ctrl + F5` to build the project and run it,
or we can right click on the solution and choose `Build`.

5. If you pick second way to build it, go back to `TutorialProject.uproject` files and double clicks it.
It will open the Unreal Editor from there.

6. Here you can observe how the project been setup. 

## Observing AccelByte Plugins and Implementation

Here we will observe several points how services in AccelByte implemented.

- Check on `Source/TutorialProject.Target.cs`, `Source/TutorialProjectEditor.Target.cs` , and 
`Source/TutorialProject/TutorialProject.Build.cs` to see how AccelByte plugins implemented.

- Check on `Config/DefaultEngine.ini` to see how Client and Server setup implemented. 
(This also can be accessed through editor by clicking Edit > Project Settings > Go to Plugins section
and click AccelByte Unreal Engine 4 Client SDK / AccelByte Unreal Engine 4 Server SDK)

For information about this services, please contact info@accelbyte.io