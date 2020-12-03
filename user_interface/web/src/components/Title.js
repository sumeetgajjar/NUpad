import React, {Component} from 'react'
import TextField from '@material-ui/core/TextField';

class Title extends Component {

    constructor(props, context) {
        super(props, context);
        this.title = "";
    }

    handleChange = (e) => {
        const docName = e.target.value;
        if (!docName || docName === "") {
            return false;
        }
        this.title = docName;
    }

    handleBlur = () => {
        this.props.onBlur(this.title);
    }

    render() {

        return (
            <TextField
                label="Document Name"
                id="outlined-margin-dense"
                className={this.props.styleClasses.textField}
                margin="dense"
                variant="outlined"
                onChange={this.handleChange}
                onBlur={this.handleBlur}
                fullWidth
                required
            />
        )
    }
}

export default Title