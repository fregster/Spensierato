function LoadVideoBar(id, load) {

      var videoBar;
      var barContainer = document.getElementById("videoBar"+id);

      var options = {
        horizontal : true,
        autoExecuteList : {
          cycleTime : GSvideoBar.CYCLE_TIME_MEDIUM,
          cycleMode : GSvideoBar.CYCLE_MODE_LINEAR,
          executeList : [ load ]
        }
      }
      videoBar = new GSvideoBar(barContainer, GSvideoBar.PLAYER_ROOT_FLOATING, options);
    };