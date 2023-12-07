import React from 'react';

let mediaStyle = {
    position: 'absolute',
    top: 0,
    left: 0,
    width: "100%",
    height: "100%",
}

export class VirtualFarm extends React.Component {
    constructor(props) {
        super(props);

        this.updateSrcs = this.updateSrcs.bind(this);
        this.getVideoToPlay = this.getVideoToPlay.bind(this);

        this.state = {
            viewSrc: null,
            viewTrans1: null,
            viewTrans2: null,
        }

        this.updateSrcs(false, false);

        this.previousView = this.props.view;
        this.transitioning = false;
    }

    updateSrcs(imageOnly = false, mounted = true) {
        let futureState = {viewSrc: "/assets/images/overview.jpeg"};
        if (this.props.view === "overview") {
            futureState = {
                viewSrc: "/assets/images/overview.jpeg",
                viewTrans1: "/assets/videos/transitionOverviewToPlants.mp4",
                viewTrans2: "/assets/videos/transitionOverviewToWater.mp4",
            };
        } else if (this.props.view === "plants") {
            futureState = {
                viewSrc: "/assets/images/plants.jpeg",
                viewTrans1: "/assets/videos/transitionPlantsToOverview.mp4",
                viewTrans2: "/assets/videos/transitionPlantsToWater.mp4",
            };
        } else if (this.props.view === "water") {
            futureState = {
                viewSrc: "/assets/images/water.jpeg",
                viewTrans1: "/assets/videos/transitionWaterToOverview.mp4",
                viewTrans2: "/assets/videos/transitionWaterToPlants.mp4",
            };
        }

        if (imageOnly) {
            document.getElementById("machineImage").src = futureState.viewSrc;
            return;
        } else if (mounted) {
            document.getElementById("videoSrc1").setAttribute("src", futureState.viewTrans1);
            document.getElementById("videoSrc2").setAttribute("src", futureState.viewTrans2);
            document.getElementById("transitionVideo1").load();
            document.getElementById("transitionVideo2").load();
        }

        if (mounted) {
            this.setState(futureState);
        } else {
            // eslint-disable-next-line react/no-direct-mutation-state
            this.state = futureState;
        }
    }

    getVideoToPlay() {
        if (this.props.view === "overview") {
            if (this.previousView === "plants") {
                return 1;
            } else if (this.previousView === "water") {
                return 1;
            }
        } else if (this.props.view === "plants") {
            if (this.previousView === "overview") {
                return 1;
            } else if (this.previousView === "water") {
                return 2;
            }
        } else if (this.props.view === "water") {
            if (this.previousView === "overview") {
                return 2;
            } else if (this.previousView === "plants") {
                return 2;
            }
        }

        return 0; // no video to play, show image
    }

    render() {
        let videoToPlay = this.getVideoToPlay();
        if (!this.transitioning && videoToPlay !== 0) {
            this.transitioning = true;
            // rewinding video
            document.getElementById("transitionVideo1").currentTime = 0;
            document.getElementById("transitionVideo2").currentTime = 0;
            this.props.onNewView(null);
            if (videoToPlay === 1) {
                document.getElementById("transitionVideo1").play();
            } else if (videoToPlay === 2) {
                document.getElementById("transitionVideo2").play();
            }
            setTimeout(() => {
                this.transitioning = false;
                this.previousView = this.props.view;
                document.getElementById("machineImage").hidden = false;
                this.updateSrcs();
                this.props.onNewView(this.props.view);
            }, 1100);
            setTimeout(() => {
                document.getElementById("machineImage").hidden = true;
                this.updateSrcs(true);
            }, 100);
        }

        return (
            <div style={this.props.style}>
                <video style={mediaStyle} id={"transitionVideo1"} muted hidden={videoToPlay !== 1}>
                    <source src={this.state.viewTrans1} id={"videoSrc1"} type="video/mp4"/>
                </video>
                <video style={mediaStyle} id={"transitionVideo2"} muted hidden={videoToPlay !== 2}>
                    <source src={this.state.viewTrans2} id={"videoSrc2"} type="video/mp4"/>
                </video>
                <img src={this.state.viewSrc} id="machineImage" style={mediaStyle} alt={"machine"}/>
            </div>
        );
    }
}