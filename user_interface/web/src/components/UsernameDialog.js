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
        this.state = {open: true};
    }

    handleUsernameInput = (e) => {
        this.setState({
            username: e.target.value
        });
    }

    handleClick = (e) => {
        if (!this.state.username) {
            return false;
        }
        this.props.onChange(this.state.username);
        this.setState({
            open: false
        });
    };

    render() {
        return (
            <div className={this.props.styleClasses.usernameDialog}>
                <Dialog open={this.state.open} aria-labelledby="form-dialog-title" fullWidth={true}
                        onEscapeKeyDown={() => false}>
                    <DialogTitle id="form-dialog-title">Enter username</DialogTitle>
                    <DialogContent>
                        <TextField
                            autoFocus
                            margin="dense"
                            id="name"
                            label="Username"
                            helperText="Username required before you start editing"
                            required
                            fullWidth
                            onChange={this.handleUsernameInput}
                        />
                    </DialogContent>
                    <DialogActions>
                        <Button onClick={this.handleClick} color="primary">
                            Start Editing
                        </Button>
                    </DialogActions>
                </Dialog>
            </div>
        );
    }
}

export default UsernameDialog;