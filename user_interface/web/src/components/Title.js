import React, {Component} from 'react'
import TextField from '@material-ui/core/TextField';

class Title extends Component {

    constructor(props, context) {
        super(props, context);
        this.state = {documentName: ""};
    }

    render() {

        return (
            <TextField
                label="Document Name"
                id="outlined-margin-dense"
                defaultValue={this.props.documentName}
                className={this.props.classes.textField}
                margin="dense"
                variant="outlined"
                fullWidth
                required
            />
        )
    }
}

export default Title