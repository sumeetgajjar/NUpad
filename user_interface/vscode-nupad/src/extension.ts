// The module 'vscode' contains the VS Code extensibility API
// Import the module and reference it with the alias vscode in your code below
import * as vscode from 'vscode';
import * as WebSocket from 'ws';

const INIT_REQUEST = 1;
const INIT_RESPONSE = 2;
const UI_INPUT = 3;
const REMOTE_CHANGE = 4;
const INSERT_OPERATION = 1;
const DELETE_OPERATION = 2;
const CONNECT_URL = 'ws://127.0.0.1:9002';

class NUpadClient {

    title: string;
    peerName: string;
    socket: WebSocket;
    remoteChange: boolean;

    constructor() {
        this.title = "demo-doc";
        this.peerName = "";
        this.socket = this.initSocket();
        this.remoteChange = false;
    }

    initSocket = () => {
        let socket = new WebSocket(CONNECT_URL);
        socket.onopen = () => {
            this.sendMessage({
                message_type: INIT_REQUEST,
                document_name: this.title
            });
        };
        socket.onclose = () => {
            console.log("closed");
        };
        socket.onmessage = this.handleRemoteChange;
        return socket;
    };

    close = ()=> {
        if (this.socket) {
            this.socket.close();
        }
    };

    sendMessage = (message: any) => {
        console.log("sending: ", message);
        this.socket.send(JSON.stringify(message));
    };

    setContent = (content: string) => {
        const editor = vscode.window.activeTextEditor;
        if (editor) {
            const document = editor.document;
            editor.edit(editBuilder => {
                const firstLine = document.lineAt(0);
                const lastLine = document.lineAt(document.lineCount - 1);
                editBuilder.delete(new vscode.Range(firstLine.range.start, lastLine.range.end));
                editBuilder.insert(firstLine.range.start, content);
            });
        } else {
            console.log("cannot find text editor");
        }
    };

    handleRemoteChange = (e: any) => {
        this.remoteChange = true;
        const msg = JSON.parse(e.data);
        console.log("Received -> ", msg);
        if (msg['messageType'] === INIT_RESPONSE) {
            this.peerName = msg['peerName'];
            this.setContent(msg['initialContent']);
        } else if (msg['messageType'] === REMOTE_CHANGE) {
            this.setContent(msg['content']);
        } else {
            console.error("unknown message received: ", msg);
        }
    };

    handleDocumentChange = (e: any) => {
        console.log("event", e);
        if (this.remoteChange) {
            this.remoteChange = false;
            return;
        }

        const editor = vscode.window.activeTextEditor;
        if (editor) {
            const cursor = e.contentChanges[0].rangeOffset;
            const value = e.contentChanges[0].text;
            if (value.length === 1) {
                // Insertion
                const ix = cursor;
                this.sendMessage({
                    'message_type': UI_INPUT,
                    'index': ix,
                    'value': value,
                    'operation_type': INSERT_OPERATION
                });
            } else if (value.length === 0) {
                // Deletion
                const ix = cursor;
                this.sendMessage({
                    'message_type': UI_INPUT,
                    'index': ix,
                    'operation_type': DELETE_OPERATION
                });
            } else {
                console.error("error");
            }
        }
    };

}

// this method is called when your extension is activated
// your extension is activated the very first time the command is executed
export function activate(context: vscode.ExtensionContext) {

    // Use the console to output diagnostic information (console.log) and errors (console.error)
    // This line of code will only be executed once when your extension is activated
    console.log('extension "vscode-nupad" is now active!');
    let nuPadClient : NUpadClient | null;
    context.subscriptions.push(vscode.commands.registerCommand('vscode-nupad.StartNUpad', () => {
        nuPadClient = new NUpadClient();
        vscode.workspace.onDidChangeTextDocument((e) => {
            if(nuPadClient) {
                nuPadClient.handleDocumentChange(e);
            }
        });
    }));

    context.subscriptions.push(vscode.commands.registerCommand('vscode-nupad.StopNUpad', () => {
        if(nuPadClient) {
            nuPadClient.close();
        }
    }));
}

// this method is called when your extension is deactivated
export function deactivate() {
}
