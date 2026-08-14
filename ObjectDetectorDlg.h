
// ObjectDetectorDlg.h : header file
//

#pragma once
#include <opencv2/opencv.hpp>

// CObjectDetectorDlg dialog
class CObjectDetectorDlg : public CDialogEx
{
// Construction
public:
	CObjectDetectorDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OBJECTDETECTOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void DisplayImage(const cv::Mat& img);

// Implementation
protected:
	HICON m_hIcon;
	cv::Mat m_currentImage;
	CString GetExeFolder();
	cv::dnn::Net m_net;
	std::vector<std::string> m_classNames;
	bool m_modelLoaded = false;
	CBrush m_bgBrush;

	bool LoadModel();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLoadButton();
	afx_msg void OnBnClickedDetectButton();
	afx_msg void OnBnClickedSaveButton();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CButton m_btnDetect;
};
