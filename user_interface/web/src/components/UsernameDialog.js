import React, {Component} from 'react'
import Button from '@material-ui/core/Button';
import TextField from '@material-ui/core/TextField';
import Dialog from '@material-ui/core/Dialog';
import DialogActions from '@material-ui/core/DialogActions';
import DialogContent from '@material-ui/core/DialogContent';
import DialogTitle from '@material-ui/core/DialogTitle';

class UsernameDialog extends Component {

    constructor(props, context) {
        super(props, context);
        this.state = {'open': true};
    }

    handleClose = () => {
        // TODO: get or set user name
        console.log("handle dialog close event")
        this.setState({
            'open': false
        })
    };


    render() {
        return (
            <div className={this.props.classes.usernameDialog}>
                <Dialog open={this.state.open} aria-labelledby="form-dialog-title" fullWidth={true}
                        onEscapeKeyDown={() => false}>
                    <DialogTitle id="form-dialog-title">Enter username</DialogTitle>
                    <DialogContent>
                        <TextField
                            autoFocus
                            margin="dense"
                            id="name"
                            label="Username"
                            required
                            fullWidth
                        />
                    </DialogContent>
                    <DialogActions>
                        <Button onClick={this.handleClose} color="primary">
                            Start Editing
                        </Button>
                    </DialogActions>
                </Dialog>
            </div>
        );
    }
}

export default UsernameDialog;