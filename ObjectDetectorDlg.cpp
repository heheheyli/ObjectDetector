
// ObjectDetectorDlg.cpp : implementation file

#include "pch.h"
#include "framework.h"
#include "ObjectDetector.h"
#include "ObjectDetectorDlg.h"
#include "afxdialogex.h"
#include <opencv4/opencv2/opencv.hpp>
#include <fstream>
#include <vector>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CObjectDetectorDlg dialog

CObjectDetectorDlg::CObjectDetectorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OBJECTDETECTOR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CObjectDetectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CObjectDetectorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOAD_BUTTON, &CObjectDetectorDlg::OnBnClickedLoadButton)
	ON_BN_CLICKED(IDC_DETECT_BUTTON, &CObjectDetectorDlg::OnBnClickedDetectButton)
	ON_BN_CLICKED(IDC_SAVE_BUTTON, &CObjectDetectorDlg::OnBnClickedSaveButton)
END_MESSAGE_MAP()


// CObjectDetectorDlg message handlers

BOOL CObjectDetectorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	cv::Mat test = cv::Mat::zeros(100, 100, CV_8UC3);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CObjectDetectorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CObjectDetectorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CObjectDetectorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CObjectDetectorDlg::OnBnClickedLoadButton()
{
	// 1. Open a file picker for image files
	CFileDialog dlg(TRUE, _T("*.jpg"), NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		_T("Image Files (*.jpg;*.jpeg;*.png;*.bmp)|*.jpg;*.jpeg;*.png;*.bmp|All Files (*.*)|*.*||"));

	if (dlg.DoModal() != IDOK)
		return; // user cancelled

	// 2. Get the chosen file path and convert to a std::string for OpenCV
	CString filePath = dlg.GetPathName();
	CStringA filePathA(filePath); // convert wide -> narrow
	std::string path(filePathA);

	// 3. Load the image with OpenCV
	cv::Mat image = cv::imread(path);
	if (image.empty())
	{
		AfxMessageBox(_T("Failed to load image."));
		return;
	}

	// 4. Store it and display it
	m_currentImage = image;
	DisplayImage(image);
}

void CObjectDetectorDlg::DisplayImage(const cv::Mat& img)
{
	if (img.empty()) return;

	// Convert to BGR if needed (OpenCV loads as BGR, which is what Windows bitmaps expect)
	cv::Mat display;
	if (img.channels() == 3)
		display = img;
	else
		cv::cvtColor(img, display, cv::COLOR_GRAY2BGR);

	// Get the picture control and its area
	CWnd* pWnd = GetDlgItem(IDC_IMAGE);
	if (!pWnd) return;

	CRect rect;
	pWnd->GetClientRect(&rect);

	// Resize to fit the control while keeping aspect ratio
	double scale = std::min(
		(double)rect.Width() / display.cols,
		(double)rect.Height() / display.rows);
	int newW = (int)(display.cols * scale);
	int newH = (int)(display.rows * scale);

	// Force width to a multiple of 4 to satisfy Windows bitmap row alignment
	newW = (newW / 4) * 4;
	if (newW < 4) newW = 4;

	cv::Mat resized;
	cv::resize(display, resized, cv::Size(newW, newH));

	// Ensure the pixel data is stored contiguously (no row padding gaps)
	if (!resized.isContinuous())
		resized = resized.clone();

	// Set up the bitmap header
	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = resized.cols;
	bmi.bmiHeader.biHeight = -resized.rows; // negative = top-down
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;

	// Draw it into the picture control
	int offsetX = (rect.Width() - newW) / 2;
	int offsetY = (rect.Height() - newH) / 2;

	CDC* pDC = pWnd->GetDC();
	pDC->FillSolidRect(&rect, RGB(240, 240, 240)); // gray background around image
	StretchDIBits(pDC->GetSafeHdc(),
		offsetX, offsetY, newW, newH,
		0, 0, resized.cols, resized.rows,
		resized.data, &bmi, DIB_RGB_COLORS, SRCCOPY);
	pWnd->ReleaseDC(pDC);
}

void CObjectDetectorDlg::OnBnClickedDetectButton()
{
	if (m_currentImage.empty()) {
		AfxMessageBox(_T("Load an image first."));
		return;
	}

	if (!LoadModel()) return; // error already shown

	const int INPUT_SIZE = 640;
	const float CONF_THRESHOLD = 0.25f;
	const float NMS_THRESHOLD = 0.45f;

	cv::Mat image = m_currentImage.clone();
	int imgW = image.cols;
	int imgH = image.rows;

	// 1. Preprocess: make a 640x640 blob
	cv::Mat blob = cv::dnn::blobFromImage(
		image, 1.0 / 255.0, cv::Size(INPUT_SIZE, INPUT_SIZE),
		cv::Scalar(), true, false);

	// 2. Run inference
	m_net.setInput(blob);
	cv::Mat output = m_net.forward();

	// 3. YOLOv8 output is [1 x 84 x 8400] -> reshape and transpose to [8400 x 84]
	//    84 = 4 box coords + 80 class scores (no separate objectness score)
	cv::Mat out = output.reshape(1, output.size[1]); // 84 x 8400
	cv::transpose(out, out);                          // 8400 x 84

	std::vector<int> classIds;
	std::vector<float> confidences;
	std::vector<cv::Rect> boxes;

	float xFactor = (float)imgW / INPUT_SIZE;
	float yFactor = (float)imgH / INPUT_SIZE;

	// 4. Parse each detection row
	for (int i = 0; i < out.rows; i++) {
		float* row = out.ptr<float>(i);
		// class scores start at index 4
		cv::Mat scores(1, (int)m_classNames.size(), CV_32FC1, row + 4);
		cv::Point classIdPoint;
		double maxScore;
		cv::minMaxLoc(scores, 0, &maxScore, 0, &classIdPoint);

		if (maxScore > CONF_THRESHOLD) {
			float cx = row[0], cy = row[1], w = row[2], h = row[3];
			int left = (int)((cx - 0.5f * w) * xFactor);
			int top = (int)((cy - 0.5f * h) * yFactor);
			int width = (int)(w * xFactor);
			int height = (int)(h * yFactor);

			classIds.push_back(classIdPoint.x);
			confidences.push_back((float)maxScore);
			boxes.push_back(cv::Rect(left, top, width, height));
		}
	}

	// 5. Non-max suppression: remove overlapping duplicate boxes
	std::vector<int> indices;
	cv::dnn::NMSBoxes(boxes, confidences, CONF_THRESHOLD, NMS_THRESHOLD, indices);

	// 6. Draw surviving boxes + labels
	for (int idx : indices) {
		cv::Rect box = boxes[idx];
		cv::rectangle(image, box, cv::Scalar(0, 255, 0), 2);

		double fontScale = std::max(0.5, image.cols / 1200.0);
		int thickness = std::max(1, (int)(fontScale * 2));

		std::string label = m_classNames[classIds[idx]] +
			" " + cv::format("%.2f", confidences[idx]);
		int baseline = 0;
		cv::Size textSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, fontScale, thickness, &baseline);
		int ty = std::max(box.y, textSize.height + 5);
		cv::rectangle(image,
			cv::Point(box.x, ty - textSize.height - 5),
			cv::Point(box.x + textSize.width, ty + baseline - 5),
			cv::Scalar(0, 255, 0), cv::FILLED);
		cv::putText(image, label, cv::Point(box.x, ty - 5),
			cv::FONT_HERSHEY_SIMPLEX, fontScale, cv::Scalar(0, 0, 0), thickness);
	}

	// 7. Store annotated result + display it
	m_currentImage = image;
	DisplayImage(image);

	if (indices.empty())
		AfxMessageBox(_T("No objects detected above the confidence threshold."));
}

CString CObjectDetectorDlg::GetExeFolder()
{
	wchar_t path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	CString exePath(path);
	int lastSlash = exePath.ReverseFind(_T('\\'));
	return exePath.Left(lastSlash + 1); // folder containing the exe, with trailing backslash
}

bool CObjectDetectorDlg::LoadModel()
{
	if (m_modelLoaded) return true; // already loaded

	CString folder = GetExeFolder();

	// Build paths to model + class names (relative to exe)
	CStringA modelPathA(folder + _T("models\\yolov8n.onnx"));
	CStringA namesPathA(folder + _T("models\\coco.names"));

	// Load the network
	try {
		m_net = cv::dnn::readNetFromONNX(std::string(modelPathA));
	}
	catch (const cv::Exception& e) {
		CStringA msg(e.what());
		AfxMessageBox(CString(_T("Failed to load model: ")) + CString(msg));
		return false;
	}

	if (m_net.empty()) {
		AfxMessageBox(_T("Model loaded but is empty."));
		return false;
	}

	// CPU backend
	m_net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
	m_net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

	// Load class names
	m_classNames.clear();
	std::string namesPath(namesPathA);
	std::ifstream ifs(namesPath);

	if (!ifs.is_open()) {
		AfxMessageBox(_T("Failed to open coco.names."));
		return false;
	}
	std::string line;
	while (std::getline(ifs, line)) {
		if (!line.empty() && line.back() == '\r') line.pop_back(); // strip Windows CR
		if (!line.empty()) m_classNames.push_back(line);
	}

	m_modelLoaded = true;
	return true;
}

void CObjectDetectorDlg::OnBnClickedSaveButton()
{
	if (m_currentImage.empty()) {
		AfxMessageBox(_T("No image to save. Load and detect first."));
		return;
	}

	// Save-file dialog, default to .jpg
	CFileDialog dlg(FALSE, _T("jpg"), _T("detected.jpg"),
		OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
		_T("JPEG (*.jpg)|*.jpg|PNG (*.png)|*.png|All Files (*.*)|*.*||"));

	if (dlg.DoModal() != IDOK)
		return; // user cancelled

	CString savePath = dlg.GetPathName();
	CStringA savePathA(savePath);

	// Write the annotated image to disk
	if (cv::imwrite(std::string(savePathA), m_currentImage))
		AfxMessageBox(_T("Image saved successfully."));
	else
		AfxMessageBox(_T("Failed to save image."));
}
