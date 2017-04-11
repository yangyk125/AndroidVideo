package com.android.video.page;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.PixelFormat;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.View;
import android.widget.AbsoluteLayout;
import android.widget.ImageView.ScaleType;
import android.widget.TextView;

import com.android.nxui.NXPageView;
import com.android.video.R;
import com.android.video.page.EFEffectPageView.EFEffectPageListener;
import com.android.video.page.widget.EFEffectFilmView;
import com.android.video.page.widget.EFEffectListView;
import com.android.video.views.EFImageButton;
import com.android.video.views.EFImageButton.EFButtonStatus;
import com.android.video.views.EFImageView;
import com.android.video.views.EFProgressView;
import com.android.video.views.EFSliderView;
import com.android.video.views.EFSurfaceView;
import com.android.video.worker.EFEditorWorker;
import com.android.video.worker.EFEditorWorker.EFEditorWorkerProgress;
import com.android.video.worker.EFEditorWorker.EFExtractWorkerProgress;
import com.android.video.worker.EFEditorWorker.EFPlayerWorkerProgress;
import com.android.video.worker.EFEditorWorker.EFRenderWorkerProgress;

import java.util.Map;

@SuppressWarnings("deprecation")
public class EFEditorPageView extends NXPageView implements SurfaceHolder.Callback, 
															View.OnClickListener,  
															View.OnTouchListener,
															EFPlayerWorkerProgress,
															EFEditorWorkerProgress,
															EFRenderWorkerProgress,
															EFExtractWorkerProgress,
															EFEffectPageListener{

	private static final int    PLAYER_VIDEO_LOADED  = 6626261 ;
	private static final int    PLAYER_VIDEO_PROGRS  = 6626262 ;
	private static final int    PLAYER_VIDEO_STARTD  = 6626263 ;
	private static final int    PLAYER_VIDEO_PAUSED  = 6626264 ;
	private static final int    PLAYER_VIDEO_STOPED  = 6626265 ;
	
	private static final int    EDITOR_DELET_SPRITE  = 6626221 ;

	private static final int    RENDER_VIDEO_START   = 6626251 ;
	private static final int    RENDER_VIDEO_PROGRS  = 6626252 ;
	private static final int    RENDER_VIDEO_FAILED  = 6626253 ;
	private static final int    RENDER_VIDEO_FINISH  = 6626254 ;
	
	private static final int    EXTRCT_VIDEO_START   = 6626271 ;
	private static final int    EXTRCT_VIDEO_INDEX   = 6626273 ;
	private static final int    EXTRCT_VIDEO_STOPD   = 6626274 ;
	
	private String				rootFolder	  = null ;
	private String				videoFile     = null ;
	
	//layouts
	private LayoutParams		headerLayout  = null ;
	private LayoutParams		pannelLayout  = null ;
	private LayoutParams		convorLayout  = null ;
	
	//editor header
	private AbsoluteLayout		editorHeader  = null ; //编辑时顶部View
	private EFImageButton		editorBacker  = null ;
	private TextView			editorTitler  = null ; //编辑时显示的标题
	private EFImageButton		editorConfrm  = null ; //编辑时生成视频
	
	private AbsoluteLayout		surfaceGroup  = null ;
	private EFImageView			surfaceBack   = null ;
	private EFSurfaceView		surfaceView   = null ; //中间显示画面的surface
	
	//editor buttons
	private AbsoluteLayout		editorPannel  = null ; //编辑时的面板。
	
	private EFSliderView		editorSlider  = null ;
	
	private AbsoluteLayout		editorCtrlrs  = null ;
	private EFImageButton 		editorEffcts  = null ;
	private EFImageButton 		editorPlayer  = null ;
	private EFImageButton 		editorVoices  = null ;
	
	private EFEditorWorker		editorWorker  = null ;
	private EFEffectFilmView	editorFilmer  = null ;
	private EFEffectListView	editorEflist  = null ;
	
	private EFProgressView 		buildProgress = null ;
	
	private float				editDuration  = 0.0f ;
	private float				editProgress  = 0.0f ;
	
	protected EFEditorPageView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
	}

	protected EFEditorPageView(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	protected EFEditorPageView(Context context) {
		super(context);
	}
	
	public EFEditorPageView(Context context, String rootFolder, String recordVideo) {
		super(context);
		
		this.rootFolder = rootFolder ;
		this.videoFile  = recordVideo ;
		
		initEditorPageView() ;
	}

	protected void initEditorPageView() {
		super.setBackgroundColor(0xFF1B1E20) ;
		
		this.headerLayout = new NXPageView.LayoutParams(headerRect.width(), headerRect.height(), headerRect.left, headerRect.top) ;
		this.pannelLayout = new NXPageView.LayoutParams(clientRect.width(), clientRect.height() -clientRect.width()-headerRect.height(), clientRect.left, clientRect.top+headerRect.height()+clientRect.width()) ;
		this.convorLayout  = new NXPageView.LayoutParams(clientRect.width(), clientRect.height(), clientRect.left, clientRect.top) ;
		
		this.surfaceGroup = new AbsoluteLayout(getContext()) ;
		this.surfaceGroup.setBackgroundColor(0xFF000000);
		this.surfaceGroup.setLayoutParams(new NXPageView.LayoutParams(clientRect.width(), clientRect.width(), clientRect.left, clientRect.top+headerRect.height())) ;
		this.addView(this.surfaceGroup) ;
		
		this.surfaceBack = new EFImageView(getContext()) ;
		this.surfaceBack.setScaleType(ScaleType.CENTER_INSIDE);
		this.surfaceBack.setImageResource(R.drawable.icon_draw_surface);
		this.surfaceBack.setLayoutParams(new NXPageView.LayoutParams(clientRect.width(), clientRect.width(), 0, 0)) ;
		this.surfaceGroup.addView(this.surfaceBack) ;
		
		this.surfaceView = new EFSurfaceView(getContext());
		this.surfaceView.setLayoutParams(new NXPageView.LayoutParams(clientRect.width(), clientRect.width(), 0, 0)) ;
		this.surfaceView.getHolder().setFormat(PixelFormat.RGBA_8888);
		this.surfaceView.getHolder().addCallback(this) ;
		this.surfaceView.setOnTouchListener(this) ;
		this.surfaceGroup.addView(this.surfaceView) ;
		
		this.initEditorViews() ;
	}
	
	protected void initEditorViews() {
		
		this.editorHeader = new AbsoluteLayout(getContext()) ;
		this.editorHeader.setBackgroundColor(0xFF1B1E20) ;
		this.editorHeader.setLayoutParams(this.headerLayout) ;
		this.addView(this.editorHeader, 0) ;
		
		this.editorBacker = new EFImageButton(getContext()) ;
		this.editorBacker.setLayoutParams(new NXPageView.LayoutParams(headerRect.height(), headerRect.height(), 0, 0)) ;
		this.editorBacker.setPadding(headerRect.height()/5, headerRect.height()/5, headerRect.height()/5, headerRect.height()/5) ;
		this.editorBacker.setScaleType(ScaleType.CENTER_INSIDE) ;
		this.editorBacker.setImageResource(EFButtonStatus.BUTTON_STATUS_NORMAL, R.drawable.btn_backer_normal) ;
		this.editorBacker.setImageResource(EFButtonStatus.BUTTON_STATUS_SELECT, R.drawable.btn_backer_select) ;
		this.editorBacker.setOnClickListener(this) ;
		this.editorHeader.addView(this.editorBacker) ;
		
		this.editorTitler = new TextView(getContext()); 
		this.editorTitler.setLayoutParams(new NXPageView.LayoutParams(headerRect.width()-3*headerRect.height(), headerRect.height(), headerRect.height()*3/2, 0)) ;
		this.editorTitler.setGravity(Gravity.CENTER_HORIZONTAL|Gravity.CENTER_VERTICAL) ;
		this.editorTitler.setTextSize(TypedValue.COMPLEX_UNIT_PX, headerRect.height()/2);
		this.editorTitler.setTextColor(0xFFEEEEEE) ;
		this.editorTitler.setText("预览编辑") ;
		this.editorHeader.addView(this.editorTitler) ;
		
		this.editorConfrm = new EFImageButton(getContext()) ;
		this.editorConfrm.setLayoutParams(new NXPageView.LayoutParams(headerRect.height(), headerRect.height(),headerRect.width()-headerRect.height(), 0)) ;
		this.editorConfrm.setScaleType(ScaleType.CENTER_INSIDE) ;
		this.editorConfrm.setPadding(headerRect.height()/5, headerRect.height()/5, headerRect.height()/5, headerRect.height()/5) ;
		this.editorConfrm.setImageResource(EFButtonStatus.BUTTON_STATUS_NORMAL, R.drawable.btn_render_normal) ;
		this.editorConfrm.setImageResource(EFButtonStatus.BUTTON_STATUS_SELECT, R.drawable.btn_render_select) ;
		this.editorConfrm.setOnClickListener(this) ;
		this.editorHeader.addView(this.editorConfrm) ;
		
		this.editorPannel = new AbsoluteLayout(getContext()) ;
		this.editorPannel.setBackgroundColor(0xFF1B1E20) ;
		this.editorPannel.setLayoutParams(this.pannelLayout) ;
		this.addView(this.editorPannel, 0) ;
		
		int top = 0 ;
		this.editorSlider = new EFSliderView(getContext(), 1.0f) ;
		this.editorSlider.setLayoutParams(new AbsoluteLayout.LayoutParams(clientRect.width(), clientRect.width()/100, 0, top)) ;
		this.editorPannel.addView(this.editorSlider) ;
		top += clientRect.width()/100 ;
		
		this.editorCtrlrs = new AbsoluteLayout(getContext()) ;
		this.editorCtrlrs.setLayoutParams(new AbsoluteLayout.LayoutParams(clientRect.width(), clientRect.width()/8, 0, top)) ;
		this.editorPannel.addView(this.editorCtrlrs) ;
		top += clientRect.width()/8 ;
		
		this.initEditorButtons() ;
		
		this.editorFilmer = new EFEffectFilmView(getContext()) ;
		this.editorFilmer.setLayoutParams(new AbsoluteLayout.LayoutParams(clientRect.width(), clientRect.width()/10, 0, top)) ;
		this.editorFilmer.assignFilmImages(9, clientRect.width(), clientRect.width()/10) ;
		this.editorPannel.addView(this.editorFilmer) ;
		top += clientRect.width()/10 ;
		
		this.editorEflist = new EFEffectListView(getContext(), clientRect.width()) ;
		this.editorEflist.setLayoutParams(new AbsoluteLayout.LayoutParams(clientRect.width(), this.pannelLayout.height-top, 0, top));
		this.editorPannel.addView(this.editorEflist) ;
	}
	
	protected void initEditorButtons() {
		
		int panelWidth = this.editorCtrlrs.getLayoutParams().width ;
		int panelHeight = this.editorCtrlrs.getLayoutParams().height ;
		
		this.editorEffcts = new EFImageButton(getContext()) ;
		this.editorEffcts.setLayoutParams(new AbsoluteLayout.LayoutParams(panelHeight, panelHeight, (panelWidth-panelHeight)/2, 0)) ;
		this.editorEffcts.setImageResource(EFButtonStatus.BUTTON_STATUS_NORMAL, R.drawable.btn_addeff_normal) ;
		this.editorEffcts.setImageResource(EFButtonStatus.BUTTON_STATUS_SELECT, R.drawable.btn_addeff_select) ;
		this.editorEffcts.setPadding(panelHeight/15, panelHeight/15, panelHeight/15, panelHeight/15) ;
		this.editorEffcts.setOnClickListener(this) ;
		this.editorCtrlrs.addView(this.editorEffcts) ;
		
		this.editorPlayer  = new EFImageButton(getContext()) ;
		this.editorPlayer.setLayoutParams(new AbsoluteLayout.LayoutParams(panelHeight, panelHeight, (panelWidth-panelHeight)/2-(int)(panelHeight*1.25f), 0)) ;
		this.editorPlayer.setImageResource(EFButtonStatus.BUTTON_STATUS_NORMAL, R.drawable.btn_player_playing) ;
		this.editorPlayer.setImageResource(EFButtonStatus.BUTTON_STATUS_SELECT, R.drawable.btn_player_paused) ;
		this.editorPlayer.setPadding(panelHeight/5, panelHeight/5, panelHeight/5, panelHeight/5) ;
		this.editorPlayer.setOnClickListener(this) ;
		this.editorCtrlrs.addView(this.editorPlayer) ;
		
		this.editorVoices  = new EFImageButton(getContext()) ;
		this.editorVoices.setLayoutParams(new AbsoluteLayout.LayoutParams(panelHeight, panelHeight, (panelWidth-panelHeight)/2+(int)(panelHeight*1.25f), 0)) ;
		this.editorVoices.setImageResource(EFButtonStatus.BUTTON_STATUS_NORMAL, R.drawable.btn_voice_open) ;
		this.editorVoices.setImageResource(EFButtonStatus.BUTTON_STATUS_SELECT, R.drawable.btn_voice_close) ;
		this.editorVoices.setPadding(panelHeight/5, panelHeight/5, panelHeight/5, panelHeight/5) ;
		this.editorVoices.setOnClickListener(this) ;
		this.editorCtrlrs.addView(this.editorVoices) ;
	}
	
	public void surfaceCreated(SurfaceHolder holder) {
		Surface surface = this.surfaceView.getHolder().getSurface() ;
		this.editorWorker = new EFEditorWorker();

		this.editorWorker.createEditorWorker(surface, this.rootFolder, this.videoFile) ;
		this.editorWorker.setEditorProgress(this) ;
		this.editorWorker.setPlayerProgress(this) ;
		this.editorWorker.loadVideoPlayer() ;
		this.editorWorker.extractSerialImages(9, this.getWidth()/10, this.getWidth()/10, this) ;
	}

	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

	}

	public void surfaceDestroyed(SurfaceHolder holder) {
		this.editorWorker.unloadVideoPlayer() ;
		this.editorWorker.cleanCocos2dXCache() ;
		this.editorWorker.destoryEditorWorker() ;
	}
	
	public void onPageViewWillShow() {
		NXPageView.LayoutParams layoutParam = (NXPageView.LayoutParams)this.surfaceView.getLayoutParams() ;
		layoutParam.x = -this.getWidth() ;
		layoutParam.y = -this.getWidth() ;
		this.surfaceView.setLayoutParams(layoutParam) ;
		super.onPageViewWillShow();
	}

	public void onPageViewDidShow() {
		NXPageView.LayoutParams layoutParam = (NXPageView.LayoutParams)this.surfaceView.getLayoutParams() ;
		layoutParam.x = 0 ;
		layoutParam.y = 0 ;
		this.surfaceView.setLayoutParams(layoutParam) ;
		if(this.getPageShows() > 1) 
			this.editorWorker.resumeEditorWorker() ;
		super.onPageViewDidShow();
	}

	public void onPageViewWillHide() {
		NXPageView.LayoutParams layoutParam = (NXPageView.LayoutParams)this.surfaceView.getLayoutParams() ;
		layoutParam.x = -this.getWidth() ;
		layoutParam.y = -this.getWidth() ;
		this.surfaceView.setLayoutParams(layoutParam) ;
		
		this.editorWorker.stopPlayVideo() ;
		this.editorWorker.pausedEditorWorker() ;
		
		super.onPageViewWillHide();
	}

	public boolean onTouch(View view, MotionEvent event) {
		if(this.surfaceView == view) 
		{
			float posx = event.getX() ;
			float posy = event.getY() ;
			
			float width  = this.surfaceView.getWidth() ;
			float height = this.surfaceView.getHeight() ;
			
			int type = 0 ;
			if(event.getAction() == MotionEvent.ACTION_DOWN)
				type = 1 ;
			else if(event.getAction() == MotionEvent.ACTION_MOVE)
				type = 2 ;
			else if(event.getAction() == MotionEvent.ACTION_UP)
				type = 3 ;
			
			float x = posx/width ;
			float y = posy/height ;
			
			this.editorWorker.updateTouchEvent(type, x, y) ;
		}
		return true ;
	}


	public void onClick(View view) {
		if(this.editorBacker == view) {
			this.popFromParent(); 
		}
		else if(this.editorEffcts == view) {
			EFEffectPageView effectsPage = new EFEffectPageView(getContext()) ;
			effectsPage.setEffectListener(this) ;
			this.editorWorker.pausePlayVideo() ;
			this.editorWorker.pauseLoopAnimation() ;
			this.showPopupPage(effectsPage) ;
		}
		else if(this.editorPlayer == view) {
			if(this.editorPlayer.getButtonStatus() == EFButtonStatus.BUTTON_STATUS_NORMAL)
				this.editorWorker.startPlayVideo() ;
			else if(this.editorPlayer.getButtonStatus() == EFButtonStatus.BUTTON_STATUS_SELECT)
				this.editorWorker.pausePlayVideo() ;
		}
		else if(this.editorVoices == view) {
			
		}
		else if(this.editorConfrm == view) {
			this.editorWorker.renderEffectVideo(this) ;
		}
	}

	public boolean handleMessage(Message msg) {
		if(msg.what == PLAYER_VIDEO_LOADED) {
			Float duration = (Float)msg.obj ;
			float seconds = duration.floatValue() ;
			this.editDuration = seconds ;
			this.editProgress = 0.0f ;
			this.editorSlider.setSliderMax(this.editDuration);
			this.editorSlider.setSliderValue(this.editProgress);
			this.editorEflist.initEffListView(this.editDuration);
		}
		else if(msg.what == PLAYER_VIDEO_PROGRS) {
			Float progress = (Float)msg.obj ;
			this.editProgress = progress.floatValue() ;
			this.editorSlider.setSliderValue(this.editProgress);
		}
		else if(msg.what == PLAYER_VIDEO_STARTD) {
			this.editorPlayer.setButtonStatus(EFButtonStatus.BUTTON_STATUS_SELECT) ;
		}
		else if(msg.what == PLAYER_VIDEO_PAUSED) {
			this.editorPlayer.setButtonStatus(EFButtonStatus.BUTTON_STATUS_NORMAL) ;
		}
		else if(msg.what == PLAYER_VIDEO_STOPED) {
			this.editorPlayer.setButtonStatus(EFButtonStatus.BUTTON_STATUS_NORMAL) ;
		}
		
		//--//--//--//--//--//--//--//--//--
		else if(msg.what == EDITOR_DELET_SPRITE) {
			int effectKey = ((Integer)msg.obj).intValue() ;
			String structJson = this.editorEflist.delEffectData(effectKey) ;
			this.editorEflist.requestLayout(); 
			this.editorWorker.syncEffectDatas(structJson) ;
		}

		//--//--//--//--//--//--//--//--//--
		else if(msg.what == RENDER_VIDEO_START) {
			this.buildProgress = new EFProgressView(getContext(), this.headerRect.height()/2) ;
			this.buildProgress.setLayoutParams(this.convorLayout) ;
			this.addView(this.buildProgress) ;
			return true ;
		}
		else if(msg.what == RENDER_VIDEO_PROGRS) {
			Float progress = (Float)msg.obj ;
			this.buildProgress.setProgress(progress.floatValue()) ;
			return true ;
		}
		else if(msg.what == RENDER_VIDEO_FAILED) {
			this.buildProgress.setProgress(0) ;
			this.removeView(this.buildProgress) ;
			return true ;
		}
		else if(msg.what == RENDER_VIDEO_FINISH) {
			this.buildProgress.setProgress(1.0f) ;
			this.removeView(this.buildProgress) ;
			this.buildProgress = null ;
			
			if(msg.obj != null) {
				String videoFile = (String)msg.obj ;
				Log.d("RENDER", "file===>" + videoFile) ;
				this.editorWorker.renderFinishVideo() ;
			}
			return true ;
		}
		
		//--//--//--//--//--//--//--//--//--
		else if(msg.what == EXTRCT_VIDEO_START) {
			//int count  = msg.arg1 ;
			//int width  = msg.arg2 >> 16 ;
			//int height = msg.arg2&0xFFFF ;
		}
		else if(msg.what == EXTRCT_VIDEO_INDEX) {
			int index  = msg.arg1 ;
			Bitmap image = (Bitmap)msg.obj ;
			this.editorFilmer.setFilmImage(index, image) ;
		}
		else if(msg.what == EXTRCT_VIDEO_STOPD) {
			this.editorFilmer.requestLayout() ;
		}
		
		return false ;
	}


	public void onEffectSelectConfirm(EFEffectPageView effectPage, Map<?, ?> effectData, String effectCache) {
		effectPage.hideSelfPopupPage() ;
		this.editorWorker.resumeLoopAnimation() ;

		String structJson = this.editorEflist.addEffectData(this.editProgress, effectData, effectCache) ;
		this.editorEflist.requestLayout(); 
		this.editorWorker.syncEffectDatas(structJson) ;
	}

	public void onEffectSelectCancel(EFEffectPageView effectPage) {
		effectPage.hideSelfPopupPage() ;
		this.editorWorker.resumeLoopAnimation() ;
	}

	// - - - - - - - -
	public void onPlayerLoadedCompleted(float duration) {
		this.getPageHandler().sendMessage(Message.obtain(null, PLAYER_VIDEO_LOADED, Float.valueOf(duration))) ;
	}
	public void onPlayerPlayingProgress(float progress) {
		this.getPageHandler().sendMessage(Message.obtain(null, PLAYER_VIDEO_PROGRS, Float.valueOf(progress))) ;
	}
	public void onPlayerPlayingStart() {
		this.getPageHandler().sendEmptyMessage(PLAYER_VIDEO_STARTD) ;
	}
	public void onPlayerPlayingPause() {
		this.getPageHandler().sendEmptyMessage(PLAYER_VIDEO_PAUSED) ;
	}
	public void onPlayerPlayingStoped() {
		this.getPageHandler().sendEmptyMessage(PLAYER_VIDEO_STOPED) ;
	}
	
	// - - - - - - - -
	public void onDeleteEffectItem(int effectKey) {
		this.getPageHandler().sendMessage(Message.obtain(null, EDITOR_DELET_SPRITE, Integer.valueOf(effectKey))) ;
	}

	// - - - - - - - -
	public void onRenderProgressBegin() {
		this.getPageHandler().sendEmptyMessage(RENDER_VIDEO_START) ;
	}
	public void onRenderProgressProgress(float progress) {
		this.getPageHandler().sendMessage(Message.obtain(null, RENDER_VIDEO_PROGRS, Float.valueOf(progress))) ;
	}
	public void onRenderProgressFailure() {
		this.getPageHandler().sendEmptyMessage(RENDER_VIDEO_FAILED) ;
	}
	public void onRenderProgressComplete(String buildFile) {
		this.getPageHandler().sendMessage(Message.obtain(null, RENDER_VIDEO_FINISH, buildFile)) ;
	}
	
	// - - - - - - - -
	public void onExtractImagesStart(int count, int width, int height) {
		this.getPageHandler().sendMessage(Message.obtain(null, EXTRCT_VIDEO_START, count, (width<<16)|height)) ;
	}
	public void onExtractImagesIndex(int index, int width, int height, int[] pixels) {
		this.getPageHandler().sendMessage(Message.obtain(null, EXTRCT_VIDEO_INDEX, index, (width<<16)|height, Bitmap.createBitmap(pixels, width, height, Config.ARGB_8888))) ;
	}
	public void onExtractImagesStop() {
		this.getPageHandler().sendEmptyMessage(EXTRCT_VIDEO_STOPD) ;
	}


	public String getPageName() {

		return "EditorPage";
	}

}
