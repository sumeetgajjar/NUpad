import React from "react";
import {makeStyles} from '@material-ui/core/styles';
import Paper from '@material-ui/core/Paper';
import Grid from '@material-ui/core/Grid';
import Container from '@material-ui/core/Container';

import Title from "./components/Title";
import TextArea from "./components/TextArea";
import Collaborators from "./components/Collaborators"
import UsernameDialog from "./components/UsernameDialog"
import './App.css';

const useStyles = makeStyles((theme) => ({
    paper: {
        padding: theme.spacing(2),
        textAlign: 'center',
        color: theme.palette.text.secondary,
    },
    textField: {
        marginLeft: theme.spacing(1),
        marginRight: theme.spacing(1)
    },
    usernameDialog: {}
}));


export default function App() {
    const classes = useStyles();

    return (
        <div className={classes.root}>
            <Container maxWidth={"lg"}>
                <Grid container spacing={3}>
                    <Grid item md={12}>
                        <Paper className={classes.paper} elevation={10}>
                            <Title documentName="" classes={classes}/>
                        </Paper>
                    </Grid>
                    <Grid item md={10}>
                        <Paper className={classes.paper} elevation={10}>
                            <TextArea/>
                        </Paper>
                    </Grid>
                    <Grid item md={2}>
                        <Paper className={classes.paper} elevation={10}>
                            <Collaborators editors={[1, 2, 3, 4]}/>
                        </Paper>
                    </Grid>
                </Grid>
            </Container>
            <UsernameDialog classes={classes}/>
        </div>
    );
}