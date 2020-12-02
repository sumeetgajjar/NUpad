import React, {Component} from 'react'
import io from 'socket.io-client';
import Container from "@material-ui/core/Container";
import Grid from "@material-ui/core/Grid";
import Paper from "@material-ui/core/Paper";
import Snackbar from '@material-ui/core/Snackbar';
import Alert from '@material-ui/lab/Alert';
import Title from "./Title";
import TextArea from "./TextArea";
import Collaborators from "./Collaborators";
import UsernameDialog from "./UsernameDialog";

// TODO: change this
const CONNECT_URL = 'ws://127.0.0.1:1234';

class Document extends Component {

    constructor(props, context) {
        super(props, context);
        this.state = {
            username: null,
            content: "",
            title: "",
            // TODO: implement this as a set
            editors: [],
            showWarning: false,
            warningMessage: ""
        };
        // TODO: change this
        // this.socket = this.initSocket();
    }

    initSocket = () => {
        const socket = io(CONNECT_URL);
        socket.on('connect', () => {

        });

        socket.on('disconnect', () => {

        });

        socket.on('close', () => {

        });

        socket.on('message', this.handleRemoteChange);
        return socket;
    }

    sendOperationToApp = (message) => {
        console.log(message);
        // TODO: change this
        // this.socket.send(message);
    }

    handleUsernameChange = (username) => {
        this.setState({...this.state, "username": username, "editors": [username]});
    }

    handleTitleChange = () => {

    }

    handleLocalChange = (e) => {
        const oldContent = this.state.content;
        const newContent = e.target.value;
        this.setState({...this.state, "content": newContent});

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
            const message = {'type': 'insert', 'ix': ix, 'value': newContent[ix]};
            this.sendOperationToApp(message)
        } else if (diffCount === -1) {
            // Deletion
            const ix = cursor;
            const message = {'type': 'delete', 'ix': ix, 'value': oldContent[ix]};
            this.sendOperationToApp(message)
        } else {
            console.log("diffCount", diffCount);
            throw new Error("AbsDiffCountMoreThanOneException");
        }
    }

    handleRemoteChange = (e) => {
        // TODO: verify this
        console.log(e);
        let data = "asd";
        this.setState({...this.state, "content": data});
    }

    render() {
        return (
            <div className={this.props.styleClasses.documentRoot}>
                <Container maxWidth={"lg"}>
                    <Grid container spacing={3}>
                        <Grid item md={12} xs={12}>
                            <Paper className={this.props.styleClasses.paper} elevation={10}>
                                <Title title={this.state.title} styleClasses={this.props.styleClasses}/>
                            </Paper>
                        </Grid>
                        <Grid item md={9} xs={12}>
                            <Paper className={this.props.styleClasses.paper} elevation={10}>
                                <TextArea content={this.state.content} onChange={this.handleLocalChange}/>
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
                <UsernameDialog styleClasses={this.props.styleClasses} onChange={this.handleUsernameChange}/>
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