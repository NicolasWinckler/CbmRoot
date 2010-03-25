void create_trd_geo()
{
	const int nofPlanes = 12;
	const float dx = 4000.0; // mm
	const float dy = 4000.0; // mm
	const float dz = 0.1; // mm
	const float zPos[] = {
			5000.0, 5060.0, 5120.0, 5180.0,
			7000.0, 7060.0, 7120.0, 7180.0,
			9000.0, 9060.0, 9120.0, 9180.0};
	const float rmin[] = {
			250.0, 250.0, 250.0, 250.0,
			362.0, 362.0, 362.0, 362.0,
			474.0, 474.0, 474.0, 474.0};
	const float rmax[] = {
			4000.0, 4000.0, 4000.0, 4000.0,
			4000.0, 4000.0, 4000.0, 4000.0,
			4000.0, 4000.0, 4000.0, 4000.0};

//	const float zPos[] = {
//			1800.0, 1860.0, 1920.0, 1980.0,
//			2100.0, 2160.0, 2220.0, 2280.0,
//			2400.0, 2460.0, 2520.0, 2580.0};
//	float rmin[nofPlanes];
//	for (Int_t i = 0; i < nofPlanes; i++){
//		rmin[i] = tan(2.5 * 3.14 / 180) * zPos[i];
//	}
//	const float rmax[] = {
//			4000.0, 4000.0, 4000.0, 4000.0,
//			4000.0, 4000.0, 4000.0, 4000.0,
//			4000.0, 4000.0, 4000.0, 4000.0};


	std::ofstream fout("../../geometry/trd_simple.geo");

	// setup with BOX
//	for (int i = 0; i < nofPlanes; i++){
//		fout << "trd" << i+1 << std::endl;
//		fout << "cave" << std::endl;
//		fout << "BOX" << std::endl;
//		fout << "silicon" << std::endl;
//		fout << -dx << " " << -dy << " " << -dz << std::endl;
//		fout << dx  << " " << -dy << " " << -dz << std::endl;
//		fout << dx  << " " << -dy << " " << dz  << std::endl;
//		fout << -dx << " " << -dy << " " << dz  << std::endl;
//		fout << -dx << " " << dy  << " " << -dz << std::endl;
//		fout <<  dx << " " << dy  << " " << -dz << std::endl;
//		fout <<  dx << " " << dy  << " " << dz << std::endl;
//		fout <<  -dx << " " << dy  << " " << dz << std::endl;
//		fout << "0. 0. " << zPos[i] << std::endl;
//		fout << "1.  0.  0.  0.  1.  0.  0.  0.  1." << std::endl;
//		fout << "//*********************************" << std::endl;
//	}

//	point 0:  	x, y, z coordinate of the center of the circle at the beginning of the cone
//	point 1: 	inner radius RMN1 at the beginning of the cone
//	  	outer radius RMX1at the beginning of the cone
//	point 2: 	x, y, z coordinate of the center of the circle at the end of the cone
//	point 3: 	inner radius RMN2 at the end of the cone
//	  	outer radius RMX2 at the end of the cone

	for (int i = 0; i < nofPlanes; i++){
		fout << "trd" << i+20 << std::endl;
		fout << "cave" << std::endl;
		fout << "CONE" << std::endl;
		fout << "silicon" << std::endl;
		fout << 0. << " " << 0. << " " << 0. << std::endl;
		fout << rmin[i] << " " << rmax[i] << std::endl;
		fout << 0. << " " << 0. << " " << dz << std::endl;
		fout << rmin[i] << " " << rmax[i] << std::endl;
		fout << "0. 0. " << zPos[i] << std::endl;
		fout << "1.  0.  0.  0.  1.  0.  0.  0.  1." << std::endl;
		fout << "//*********************************" << std::endl;
	}

	fout.close();
}
