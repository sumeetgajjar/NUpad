import React, {Component} from 'react'
import Avatar from '@material-ui/core/Avatar';
import Chip from '@material-ui/core/Chip';
import Divider from '@material-ui/core/Divider';

class Collaborators extends Component {


    constructor(props, context) {
        super(props, context);
        this.chipColors = ["primary", "secondary", "default"];
    }

    getChipColor = (editor, ix) => {
        return this.chipColors[ix % this.chipColors.length];
    }

    render() {
        return (
            <div>
                <h3>Collaborators</h3>
                <Divider/>
                <div className={this.props.styleClasses.editorDiv}>
                    {this.props.editors.sort().map((editor, ix) => {
                        return (
                            <Chip
                                key={editor}
                                avatar={<Avatar>{editor[0]}</Avatar>}
                                label={editor}
                                clickable
                                color={this.getChipColor(editor, ix)}
                            />
                        );
                    })}
                </div>
            </div>
        );
    }
}

export default Collaborators