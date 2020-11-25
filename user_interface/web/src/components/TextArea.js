import React, {Component} from 'react'
import TextField from "@material-ui/core/TextField";

class TextArea extends Component {


    render() {
        return (
            <TextField
                id="outlined-multiline-static"
                label="Content"
                multiline
                rows={20}
                variant="outlined"
                fullWidth
                value={this.props.content}
                onChange={this.props.onChange}
            />
        )
    }
}

export default TextArea;