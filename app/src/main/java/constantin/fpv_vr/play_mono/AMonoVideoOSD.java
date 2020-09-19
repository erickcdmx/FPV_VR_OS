package constantin.fpv_vr.play_mono;

import android.graphics.PixelFormat;
import android.os.Bundle;
import android.view.ContextMenu;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;

import androidx.appcompat.app.AppCompatActivity;

import com.google.vr.ndk.base.GvrApi;

import constantin.fpv_vr.AirHeadTrackingSender;
import constantin.fpv_vr.R;
import constantin.fpv_vr.connect.AConnect;
import constantin.fpv_vr.databinding.ActivityMonoVidOsdBinding;
import constantin.fpv_vr.djiintegration.DJIApplication;
import constantin.fpv_vr.djiintegration.TelemetryReceiverDJI;
import constantin.fpv_vr.djiintegration.VideoPlayerDJI;
import constantin.fpv_vr.settings.SJ;
import constantin.renderingx.core.FullscreenHelper;
import constantin.renderingx.core.xglview.XGLSurfaceView;
import constantin.renderingx.core.xglview.XSurfaceParams;
import constantin.telemetry.core.TelemetryReceiver;
import constantin.uvcintegration.UVCPlayer;
import constantin.video.core.DecodingInfo;
import constantin.video.core.IVideoParamsChanged;
import constantin.video.core.player.VideoPlayer;
import constantin.video.core.player.VideoSettings;

import static constantin.video.core.player.VideoSettings.VIDEO_MODE_2D_MONOSCOPIC;

/*****************************************************************
 *  * OSD can be fully disabled
 * Mode 1:
 * Play video blended with OSD in a Mono window (e.g. for Tablets usw)
 * The video is displayed with the Android HW composer, not OpenGL
 * Mode 2:
 * Play video blended with OSD in a Mono window, but without using the HW composer for video (e.g. using SurfaceTexture for it)
 * 360° or stereo video needs to be rendered with OpenGL regardless weather the user wants to see the OSD or not
 ***************************************************************** */

public class AMonoVideoOSD extends AppCompatActivity implements IVideoParamsChanged {
    private ActivityMonoVidOsdBinding binding;
    public static final String EXTRA_KEY_ENABLE_OSD="EXTRA_KEY_ENABLE_OSD";
    private TelemetryReceiver telemetryReceiver;
    private GLRMono mGLRenderer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityMonoVidOsdBinding.inflate(getLayoutInflater());
        binding.myVRLayout.setVrOverlayEnabled(false);
        // OSD is optional (e.g. 'only video' )
        final boolean ENABLE_OSD = getIntent().getBooleanExtra(EXTRA_KEY_ENABLE_OSD, true);
        // Use android surface if 'normal' video in monoscopic view
        final int VIDEO_MODE=VideoSettings.videoMode(this);
        final boolean USE_ANDROID_SURFACE_FOR_VIDEO=VIDEO_MODE== VIDEO_MODE_2D_MONOSCOPIC;
        //System.out.println("USE_ANDROID_SURFACE_FOR_VIDEO"+USE_ANDROID_SURFACE_FOR_VIDEO);
        // The video player can be configured both for android surface and opengl surface
        final UVCPlayer uvcPlayer;
        final VideoPlayer videoPlayer;
        if(SJ.getConnectionType(this)== AConnect.CONNECTION_TYPE_UVC){
            uvcPlayer=new UVCPlayer(this);
            uvcPlayer.setIVideoParamsChanged(this);
            videoPlayer=null;
            telemetryReceiver=new TelemetryReceiver(this,0,0);
        }else{
            uvcPlayer=null;
            videoPlayer= DJIApplication.isDJIEnabled(this) ?
                    new VideoPlayerDJI(this):
                    new VideoPlayer(this);
            videoPlayer.setIVideoParamsChanged(this);
            telemetryReceiver= DJIApplication.isDJIEnabled(this) ?
                    new TelemetryReceiverDJI(this,videoPlayer.getExternalGroundRecorder(),videoPlayer.getExternalFilePlayer()):
                    new TelemetryReceiver(this,videoPlayer.getExternalGroundRecorder(),videoPlayer.getExternalFilePlayer());
        }
        // if needed, create and initialize the GLSurfaceView
        XGLSurfaceView mGLSurfaceView;
        if(!USE_ANDROID_SURFACE_FOR_VIDEO || ENABLE_OSD){
            mGLSurfaceView=new XGLSurfaceView(this);
            mGLSurfaceView.setVisibility(View.VISIBLE);
            //Do not use MSAA in mono mode
            mGLSurfaceView.setEGLConfigPrams(XSurfaceParams.RGBA(0,false));
            // make transparent when using android surface for video
            if(USE_ANDROID_SURFACE_FOR_VIDEO){
                mGLSurfaceView.getHolder().setFormat(PixelFormat.TRANSLUCENT);
                mGLSurfaceView.setZOrderMediaOverlay(true);
            }
            mGLRenderer = new GLRMono(this,  telemetryReceiver, binding.myVRLayout.getGvrApi(),VIDEO_MODE, ENABLE_OSD);
            if(VIDEO_MODE== VIDEO_MODE_2D_MONOSCOPIC){
                mGLSurfaceView.setRenderer(mGLRenderer,null);
            }else{
                mGLSurfaceView.setRenderer(mGLRenderer,videoPlayer.configure2());
            }
            binding.myVRLayout.setPresentationView(mGLSurfaceView);
        }
        if(USE_ANDROID_SURFACE_FOR_VIDEO){
            binding.SurfaceViewMonoscopicVideo.setVisibility(View.VISIBLE);
            binding.SurfaceViewMonoscopicVideo.getHolder().addCallback(uvcPlayer==null ? videoPlayer.configure1() : uvcPlayer.configure1());
            //binding.SurfaceViewMonoscopicVideo.getHolder().setFormat();
        }else{
            //mGLRenderer.getVideoSurfaceHolder().setCallBack(uvcPlayer==null ? videoPlayer.configure2() : uvcPlayer.configure2());
            registerForContextMenu(binding.myVRLayout);
        }
        AirHeadTrackingSender airHeadTrackingSender = AirHeadTrackingSender.createIfEnabled(this, binding.myVRLayout.getGvrApi());
        setContentView(binding.getRoot());
    }


    @Override
    protected void onResume() {
        super.onResume();
        FullscreenHelper.setImmersiveSticky(this);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    @Override
    protected void onDestroy(){
        super.onDestroy();
    }


    @Override
    public void onVideoRatioChanged(final int videoW, final int videoH) {
        //System.out.println("Width:"+videoW+"Height:"+videoH);
        runOnUiThread(new Runnable() {
            public void run() {
                binding.VideoSurfaceAFL.setAspectRatio((double) videoW / videoH);
            }
        });
        if(mGLRenderer!=null){
            mGLRenderer.setVideoRatio(videoW,videoH);
        }
    }

    @Override
    public void onDecodingInfoChanged(DecodingInfo decodingInfo) {
        if(telemetryReceiver!=null){
            telemetryReceiver.setDecodingInfo(decodingInfo.currentFPS,decodingInfo.currentKiloBitsPerSecond,decodingInfo.avgParsingTime_ms,decodingInfo.avgWaitForInputBTime_ms,
                    decodingInfo.avgHWDecodingTime_ms);
        }
    }

    @Override
    public void onCreateContextMenu(ContextMenu menu, View v, ContextMenu.ContextMenuInfo menuInfo) {
        super.onCreateContextMenu(menu, v, menuInfo);
        menu.setHeaderTitle("Options");
        getMenuInflater().inflate(R.menu.video360_context_menu, menu);
    }

    @Override
    public boolean onContextItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.option_set_home:
                mGLRenderer.setHomeOrientation();
                return true;
            case R.id.option_goto_home:
                //mGLRenderer14Mono360.goToHomeOrientation();
                GvrApi api= binding.myVRLayout.getGvrApi();
                api.recenterTracking();
                return true;
            default:
                return super.onContextItemSelected(item);
        }
    }

}


