import React, {Component} from 'react'
import TextField from '@material-ui/core/TextField';

class Title extends Component {

    constructor(props, context) {
        super(props, context);
        this.state = {documentName: ""};
    }

    handleOnChange = (e) => {
        this.setState({
            documentName: e.target.value
        });
    }

    render() {

        return (
            <TextField
                label="Document Name"
                id="outlined-margin-dense"
                value={this.props.title}
                className={this.props.styleClasses.textField}
                margin="dense"
                variant="outlined"
                onChange={this.handleOnChange}
                fullWidth
                required
            />
        )
    }
}

export default Title