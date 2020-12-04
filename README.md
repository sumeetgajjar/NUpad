![build and run tests](https://github.com/sumeetgajjar/NUpad/workflows/build%20and%20run%20tests/badge.svg)

# NUpad
This repository contains implementation of a collaborative editor using Conflict Free Replicated Datatype.

## Demo

![](docs/nupad-demo.gif)


#### Docker build

```shell script
git clone https://github.com/sumeetgajjar/NUpad
cd NUpad
git submodule update --init --recursive
docker build . -t nupad
```

#### Running unit tests
```shell script
docker run --rm nupad run_all_tests
```

#### Running the App
**Note:** Requires docker-compose for running the app, installation instructions for docker-compose can be found
[here](https://docs.docker.com/compose/install/).


```shell script
docker-compose up -d
```

This will start the following five docker containers:

- nupadapp: Run the app instance which publishes changes to [nsq](https://nsq.io/) queue and consumes remote changes
- nupadwebui: Hosts the WebUI for editing
- nsqlookupd: Lookup service for nsq
- nsqd: nsq service
- nsqadmin: Admin interface to nsq

#### Using WebUI for editing documents

##### Steps to start WebUI
1. Once all docker containers are up, goto http://localhost:8080
2. enter a document name, e.g. `testing-doc-1`
3. Open a new browser window, goto http://localhost:8080
4. enter the same document name you entered in the step 2. i.e. `testing-doc-1`
5. start editing in both the windows  

#### Using VSCode extension for editing documents
**Note:** The vscode extension is just for demo purpose, it is not as robust as WebUI.
It has following assumptions and violating any of the following assumptions will result
into undefined behavior, might even crash all docker containers

- undo and redo does not work
- only single character should be inserted or deleted at a time, multi-character insertion and
deletion(ctrl+v to insert more than one character, selecting more than one character and
deleting) is not constrained via code. User should ensure only single character is edited at any
given time. 
- the name of the document collaborative editing is hard-coded to "demo-doc" inside extension,
thus only "demo-doc" can be edited using vscode extension

##### Steps to start vscode extension 
- Once all docker containers are up, run the following command
1. open extension code in vscode
    ```shell script
    # make sure you are in the NUpad directory
    # Goto the vscode extension source code
    cd user_interface/vscode-nupad/
    # start vscode
    code .
    ```
2. running extension using vscode
    1. press `F5` to compile and run the extension in a new "Extension Development Host" window
    2. Once the new vscode window opens
        1. Open a new file; press `ctrl+N`
        2. Click in the new file and make sure the cursor is blinking
        3. press `ctrl+shift+P` to open command palette
        4. type "Start NUpad" in the command palette, select it and press `enter`; This will
        connect the vscode extension to the NUpad app running inside the docker container
        5. open WebUI and enter document name as "demo-doc"; start typing in the content box
        6. You can see the changes in the vscode
        7. make changes in the vscode, you can see the changes in WebUI
        8. To stop the collaborative editing:
            1. press `ctrl+shift+P` to open command palette
            2. type "Stop NUpad" in the command palette, select it and press `enter`; This will
            disconnect the vscode extension from the NUpad app running inside the docker container
        