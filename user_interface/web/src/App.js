import React from "react";
import {makeStyles} from '@material-ui/core/styles';
import Document from "./components/Document";

const useStyles = makeStyles((theme) => ({
    documentRoot: {
        marginTop: theme.spacing(3)
    },
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
    const styleClasses = useStyles();

    return (
        <Document styleClasses={styleClasses}/>
    );
}