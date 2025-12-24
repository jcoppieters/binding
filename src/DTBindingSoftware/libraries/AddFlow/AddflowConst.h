typedef enum {
        afPolyline,
        afBezier,
        afVH = 2,
        afHV = 3,
        afVHV = 4,
        afHVH = 5,
        afVHVH = 6,
        afHVHV = 7,
        afVHVHV = 8,
        afHVHVH = 9,
        afVHVHVH = 10,
        afHVHVHV = 11,
        afVHVHVHV = 12,
        afHVHVHVH = 13
} LinkStyleConstants;

typedef enum {
        afNoBorder = 0,
        afFixedSingle = 1
} BorderStyleConstants;
    
typedef enum {
        afDefault = 0,
        afArrow = 1,
        afCross = 2,
        afIBeam = 3,
        afIcon = 4,
        afSize = 5,
        afSizeNESW = 6,
        afSizeNS = 7,
        afSizeNWSE = 8,
        afSizeEW = 9,
        afUpArrow = 10,
        afHourglass = 11,
        afNoDrop = 12,
		    afArrowHourglass = 13,
	      afArrowQuestion = 14,
	      afSizeAll = 15,
        afCustom = 99
} MousePointerConstants;
    
typedef enum {
        afEllipse= 0,
        afRectangle = 1,
        afRoundRect = 2,           
        afLosange = 3,
        afNorthTriangle = 4, 
        afSouthTriangle = 5, 
        afEastTriangle = 6, 
        afWestTriangle = 7, 
        afHexagon = 8, 
        afRectEdgeBump = 9,
        afRectEdgeEtched = 10,
        afRectEdgeRaised = 11,
        afRectEdgeSunken = 12,
		    afTriangleRectangle = 13,
        afPentagon = 14, 
        afOctogon = 15, 
        afProcess = 50,
        afAlternateProcess = 51,
        afDecision = 52,
        afData = 53,
        afPredefinedProcess = 54,
        afInternalStorage = 55,
        afDocument = 56,
        afMultiDocument = 57,
        afTermination = 58,
        afPreparation = 59,
        afManualInput = 60,
        afManualOperation = 61,
        afConnector = 62,
        afOffPageConnection = 63,
        afCard = 64,
        afPunchedTape = 65,
        afSummingJunction = 66,
        afOr = 67,
        afCollate = 68,
        afSort = 69,
        afExtract = 70,
        afMerge = 71,
        afStoredData = 72,
        afDelay = 73,
        afSequentialAccessStorage = 74,
        afMagneticDisk = 75,
        afDirectAccessStorage = 76,
        afDisplay = 77,
        afTransport = 78,
        afProcessIso9000 = 79,
} ShapeConstants;

typedef enum {
        afAngle0,
        afAngle90,
        afAngle180,          
        afAngle270
} ShapeOrientationConstants;

typedef enum {
        afNoScroll= 0,
        afHorizontal = 1,
        afVertical = 2,           
        afBoth = 3
} ScrollBarsConstants;

typedef enum {
        afSolid = 0,
        afDash = 1,
        afDot = 2,           
        afDashDot = 3,       
        afDashDotDot = 4,    
        afTransparent = 5,    
        afInsideSolid = 6
} DrawStyleConstants;

typedef enum {
        afLeftJustifyTOP = 0,
        afLeftJustifyMIDDLE = 1,
        afLeftJustifyBOTTOM = 2,           
        afRightJustifyTOP = 3,       
        afRightJustifyMIDDLE = 4,    
        afRightJustifyBOTTOM = 5,    
        afCenterTOP = 6,               
        afCenterMIDDLE = 7,
        afCenterBOTTOM = 8
} AlignmentConstants;                                 

typedef enum {
        afNoArrow = 0,   
        afFilledCircle = 1,          
        afEmptyCircle = 2,       
        afFilledArrow15 = 3,
        afEmptyArrow15 = 4, 
        afFilledArrow30 = 5,    
        afEmptyArrow30 = 6,    
        afFilledArrow45 = 7,               
        afEmptyArrow45 = 8,                
        afFilledFork = 9,            
        afEmptyFork = 10,                  
        afFilledClosedFork = 11,           
        afEmptyClosedFork = 12,            
        afFilledLosange = 13,          
        afEmptyLosange = 14,            
        afMany = 15,                   
        afManyOptional = 16,          
        afOne = 17,           
        afOneOptional = 18,   
        afOneOrMany = 19                  
} ArrowConstants;

typedef enum {
        afNoAutoSize= 0,
        afAdjustPictureSizetoNode = 1,
        afAdjustNodeSizetoPicture = 2,  
        afAdjustNodeSizetoText = 3     
} AutoSizeConstants;

typedef enum {
        afNoUserAction= 0,
        afNodeCreation = 1,
       	afLinkCreation = 2,
        afNodeDragging = 3,   
      	afNodeResizing = 4,   
        afLinkStretching = 5,   
        afNodeSelection = 6      
} UserActionConstants;

typedef enum {
        afNWSEsizeHandle = 0,
        afNSsizeHandle= 1,
        afNESWsizeHandle= 2,
        afWEsizeHandle = 3,
        afStretchingHandle= 4,
        afLinkingHandle= 5,
        afNodeArea= 6,
        afOutSide= 7,
        afLinkArea= 8
} PointedAreaConstants;

typedef enum {
        afTypeMediumFile = 0,
        afTypeMediumClipboard = 1
} SaveImageMediumConstants;

typedef enum {
        afWMF = 0,
        afEMF= 1
} SaveImageFormatConstants;

typedef enum {
        afNoSelection = 0,
        afSelectItems = 1,
        afZoomRectangle = 2,
        afZoomRectIso = 3
} SelectActionConstants;

typedef enum {
        afPixels = 0,
        afLines = 1,
        afDottedLines = 2
} GridStyleConstants;

typedef enum {
        afAllItems = 0,
        afSelectedItems = 1,
        afMarkedItems = 2
} ItemSetConstants;

typedef enum {
        afPictureLeftTOP = 0,
        afPictureLeftMIDDLE = 1,
        afPictureLeftBOTTOM = 2,           
        afPictureRightTOP = 3,       
        afPictureRightMIDDLE = 4,    
        afPictureRightBOTTOM = 5,    
        afPictureCenterTOP = 6,               
        afPictureCenterMIDDLE = 7,
        afPictureCenterBOTTOM = 8,
        afRelativeToText = 9
} PicturePositionConstants; 
	                                
typedef enum {
        afCenterNodeHandle = 0,
        afAllNodeArea= 1
} LinkCreationModeConstants;