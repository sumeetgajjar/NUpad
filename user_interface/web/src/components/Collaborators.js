import React, {Component} from 'react'
import Avatar from '@material-ui/core/Avatar';
import Chip from '@material-ui/core/Chip';
import Divider from '@material-ui/core/Divider';

class Collaborators extends Component {


    constructor(props, context) {
        super(props, context);
        this.chipColors = ["primary", "secondary", "default"];
        this.chipColorIndex = 0;
        this.editorsChipColor = {};
    }

    getChipColor = (editor) => {
        if (!(editor in this.editorsChipColor)) {
            this.editorsChipColor[editor] = this.chipColors[this.chipColorIndex++ % this.chipColors.length];
        }
        return this.editorsChipColor[editor];
    }

    render() {
        return (
            <div>
                <h3>Collaborators</h3>
                <Divider/>
                <div className={this.props.styleClasses.editorDiv}>
                    {this.props.editors.map(editor => {
                        return (
                            <Chip
                                key={editor}
                                avatar={<Avatar>{editor[0]}</Avatar>}
                                label={editor}
                                clickable
                                color={this.getChipColor(editor)}
                            />
                        );
                    })}
                </div>
            </div>
        );
    }
}

export default Collaborators