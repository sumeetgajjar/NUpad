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
    editorDiv: {
        display: 'flex',
        justifyContent: 'center',
        flexWrap: 'wrap',
        marginTop: theme.spacing(1),
        '& > *': {
            margin: theme.spacing(0.5),
        }
    }
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
                    <Grid item md={9}>
                        <Paper className={classes.paper} elevation={10}>
                            <TextArea/>
                        </Paper>
                    </Grid>
                    <Grid item md={3}>
                        <Paper className={classes.paper} elevation={10}>
                            <Collaborators editors={["George", "Bill", "Trump", "Obama"]} classes={classes}/>
                        </Paper>
                    </Grid>
                </Grid>
            </Container>
            <UsernameDialog classes={classes}/>
        </div>
    );
}