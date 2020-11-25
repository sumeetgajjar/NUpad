import React, {Component} from 'react'
import Avatar from '@material-ui/core/Avatar';
import Chip from '@material-ui/core/Chip';
import Divider from '@material-ui/core/Divider';

class Collaborators extends Component {


    constructor(props, context) {
        super(props, context);
        this.chipColors = ["primary", "secondary"];
        this.chipColorIndex = 0;
    }

    getChipColor = () => {
        return this.chipColors[this.chipColorIndex++ % this.chipColors.length];
    }

    render() {
        return (
            <div>
                <h3>Collaborators</h3>
                <Divider/>
                <div className={this.props.classes.editorDiv}>
                    {this.props.editors.map(editor => {
                        return (
                            <Chip
                                avatar={<Avatar>{editor[0]}</Avatar>}
                                label={editor}
                                clickable
                                color={this.getChipColor()}
                            />
                        );
                    })}
                </div>
            </div>
        );
    }
}

export default Collaborators