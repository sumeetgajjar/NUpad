import React, {Component} from 'react'

class Collaborators extends Component {

    render() {
        return (
            <div>
                <h4>Collaborators</h4>
                <ul>
                    {this.props.editors.map(editor => <li>{editor}</li>)}
                </ul>
            </div>
        );
    }
}

export default Collaborators