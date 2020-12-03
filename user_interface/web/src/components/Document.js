import React, {Component} from 'react'
import Container from "@material-ui/core/Container";
import Grid from "@material-ui/core/Grid";
import Paper from "@material-ui/core/Paper";
import Snackbar from '@material-ui/core/Snackbar';
import Alert from '@material-ui/lab/Alert';
import Title from "./Title";
import TextArea from "./TextArea";
import Collaborators from "./Collaborators";

const INIT_REQUEST = 1;
const INIT_RESPONSE = 2;
const UI_INPUT = 3;
const REMOTE_CHANGE = 4;
const INSERT_OPERATION = 1;
const DELETE_OPERATION = 2;
const CONNECT_URL = 'ws://127.0.0.1:9002';
const INITIAL_STATE = {
    content: "",
    editors: [],
    showWarning: false,
    warningMessage: "",
    textAreaDisabled: true
};

class Document extends Component {

    constructor(props, context) {
        super(props, context);
        this.title = "";
        this.peerName = "";
        this.state = {...INITIAL_STATE};
    }

    initSocket = () => {
        if (this.socket) {
            this.socket.close();
        }

        this.socket = new WebSocket(CONNECT_URL);
        this.socket.onopen = () => {
            this.sendMessage({
                message_type: INIT_REQUEST,
                document_name: this.title
            })
        };
        this.socket.onclose = () => {
            console.log("closed");
        };
        this.socket.onmessage = this.handleRemoteChange;
    }

    sendMessage = (message) => {
        console.log("sending: ", message);
        this.socket.send(JSON.stringify(message));
    }

    handleTitleChange = (title) => {
        if (title !== this.title) {
            this.setState({...this.state, content: "", textAreaDisabled: false});
            this.title = title;
            console.log("title: ", title);
            this.initSocket();
        }
    }

    handleLocalChange = (e) => {
        const oldContent = this.state.content;
        const newContent = e.target.value;
        let editors = this.state.editors;
        if (editors.findIndex(e => e === this.peerName) === -1) {
            editors.push(this.peerName);
        }
        this.setState({...this.state, "content": newContent, "editors": [...editors]});

        const diffCount = newContent.length - oldContent.length;
        if (Math.abs(diffCount) > 1) {
            const warningMessage = "Multi-" + (diffCount > 0 ? "insertion" : "deletion") + " is disabled";
            this.setState({...this.state, "showWarning": true, "warningMessage": warningMessage});
            return;
        }

        const cursor = e.target.selectionEnd;
        console.log("cursor: ", cursor);
        if (diffCount === 1) {
            // Insertion
            const ix = cursor - 1;
            this.sendMessage({
                'message_type': UI_INPUT,
                'index': ix,
                'value': newContent[ix],
                'operation_type': INSERT_OPERATION
            })
        } else if (diffCount === -1) {
            // Deletion
            const ix = cursor;
            this.sendMessage({
                'message_type': UI_INPUT,
                'index': ix,
                'value': oldContent[ix],
                'operation_type': DELETE_OPERATION
            })
        } else {
            console.log("diffCount", diffCount);
            throw new Error("AbsDiffCountMoreThanOneException");
        }
    }

    handleRemoteChange = (e) => {
        const msg = JSON.parse(e.data);
        console.log("Received -> ", msg);
        if (msg['messageType'] === INIT_RESPONSE) {
            this.peerName = msg['peerName'];
            this.setState({
                ...this.state,
                "editors": [...msg['editors']],
                "content": msg['initialContent'],
                textAreaDisabled: false
            });
        } else if (msg['messageType'] === REMOTE_CHANGE) {
            this.setState({
                ...this.state,
                "editors": [...msg['editors']],
                "content": msg['content']
            });
        } else {
            console.error("unknown message received: ", msg);
        }
    }

    render() {
        return (
            <div className={this.props.styleClasses.documentRoot}>
                <Container maxWidth={"lg"}>
                    <Grid container spacing={3}>
                        <Grid item md={12} xs={12}>
                            <Paper className={this.props.styleClasses.paper} elevation={10}>
                                <Title styleClasses={this.props.styleClasses} onBlur={this.handleTitleChange}/>
                            </Paper>
                        </Grid>
                        <Grid item md={9} xs={12}>
                            <Paper className={this.props.styleClasses.paper} elevation={10}>
                                <TextArea content={this.state.content} onChange={this.handleLocalChange}
                                          disabled={this.state.textAreaDisabled}/>
                            </Paper>
                        </Grid>
                        <Grid item md={3} xs={12}>
                            <Paper className={this.props.styleClasses.paper} elevation={10}>
                                <Collaborators editors={this.state.editors}
                                               styleClasses={this.props.styleClasses}/>
                            </Paper>
                        </Grid>
                    </Grid>
                </Container>
                <Snackbar open={this.state.showWarning} autoHideDuration={3000}
                          anchorOrigin={{vertical: "top", horizontal: "center"}}
                          onClose={() => {
                              this.setState({...this.state, "showWarning": false});
                          }}>
                    <Alert severity="warning" elevation={6} variant="filled">
                        {this.state.warningMessage}
                    </Alert>
                </Snackbar>
            </div>
        );
    }
}

export default Document;