package constantin.fpv_vr.play_stereo;
/* ************************************************************************
 * Renders Video & OSD Side by Side.
 * Pipeline h.264-->image on screen:
 * h.264 nalus->VideoDecoder->SurfaceTexture-(updateTexImage)->Texture->Rendering with OpenGL
 ***************************************************************************/

import android.graphics.SurfaceTexture;
import android.os.Bundle;
import android.view.SurfaceHolder;

import constantin.fpv_vr.AirHeadTrackingSender;
import constantin.fpv_vr.VideoTelemetryComponent;
import constantin.fpv_vr.connect.AConnect;
import constantin.fpv_vr.djiintegration.DJIApplication;
import constantin.fpv_vr.djiintegration.TelemetryReceiverDJI;
import constantin.fpv_vr.djiintegration.VideoPlayerDJI;
import constantin.fpv_vr.settings.SJ;
import constantin.renderingx.core.views.VrActivity;
import constantin.renderingx.core.views.VrView;
import constantin.renderingx.core.vrsettings.ASettingsVR;
import constantin.telemetry.core.TelemetryReceiver;
import constantin.uvcintegration.UVCPlayer;
import constantin.video.core.player.VideoPlayer;

public class AStereoVR extends VrActivity {
    //Components use the android LifecycleObserver. Since they don't need forwarding of
    //onPause / onResume it looks so empty here

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        VrView vrView=new VrView(this);
        if(ASettingsVR.getVR_RENDERING_MODE(this)>=2){
            vrView.enableSuperSync();
        }
        final VideoTelemetryComponent videoTelemetryComponent=new VideoTelemetryComponent(this);
        final GLRStereoVR mGLRStereoVR = new GLRStereoVR(this,videoTelemetryComponent.getTelemetryReceiver(),vrView.getGvrApi().getNativeGvrContext());
        videoTelemetryComponent.setIVideoParamsChanged(mGLRStereoVR);

        vrView.getPresentationView().setRenderer(mGLRStereoVR,videoTelemetryComponent.configure2());
        vrView.getPresentationView().setmISecondaryContext(mGLRStereoVR);

        vrView.setIOnEmulateTrigger(new VrView.IEmulateTrigger(){
            @Override
            public void onEmulateTrigger() {
                videoTelemetryComponent.getTelemetryReceiver().incrementOsdViewMode();
            }
        });


        setContentView(vrView);
        AirHeadTrackingSender airHeadTrackingSender = AirHeadTrackingSender.createIfEnabled(this,vrView.getGvrApi());
    }

    @Override
    protected void onResume(){
        super.onResume();
        //Debug.startMethodTracing();
    }

    @Override
    protected void onPause(){
        super.onPause();
        //Debug.stopMethodTracing();
    }

}