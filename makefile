IDIR=/usr/local/Cellar/opencv/3.3.0_3/include

LDIR=/usr/local/Cellar/opencv/3.3.0_3/lib

LIBS=-lopencv_stitching -lopencv_superres -lopencv_videostab -lopencv_photo -lopencv_aruco -lopencv_bgsegm -lopencv_bioinspired -lopencv_ccalib -lopencv_dpm -lopencv_face -lopencv_fuzzy -lopencv_img_hash -lopencv_line_descriptor -lopencv_optflow -lopencv_reg -lopencv_rgbd -lopencv_saliency -lopencv_stereo -lopencv_structured_light -lopencv_phase_unwrapping -lopencv_surface_matching -lopencv_tracking -lopencv_datasets -lopencv_text -lopencv_dnn -lopencv_plot -lopencv_ml -lopencv_xfeatures2d -lopencv_shape -lopencv_video -lopencv_ximgproc -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_flann -lopencv_xobjdetect -lopencv_imgcodecs -lopencv_objdetect -lopencv_xphoto -lopencv_imgproc -lopencv_core

videotrack: videotrack.cpp
	g++ -o videotrack videotrack.cpp -I$(IDIR)/opencv -I$(IDIR) -L$(LDIR) $(LIBS)

videocrop: videocrop.cpp
	g++ -o videocrop videocrop.cpp -I$(IDIR)/opencv -I$(IDIR) -L$(LDIR) $(LIBS)

all: videotrack videocrop

clean:
	$(RM) videotrack videocrop
