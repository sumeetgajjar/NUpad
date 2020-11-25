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
            />
        )
    }
}

export default TextArea;