# RUN THIS SCRIPT IN THE DIRECTORY WHERE THE SCRIPT LIVES AND MAKE SURE THE
# MACROS DIRECTORY EXISTS!

GEOMETRY_FILE_STR_ARR = [
    "/det/setHalfThickness RecoilE 500.0 um",
    "/det/setZ RecoilE 0.0 cm",
    "",
    "/det/setHalfThickness RecoilDE 250.0 um",
    "/det/setZ RecoilDE -1.0 cm",
    "",
    "/det/setHalfThickness Stopper 0.889 mm",
    "/det/setZ Stopper 0. cm",
    "",
    "/det/setHalfThickness LaBr 0.0 cm"
]

def main():
    MINIMUM_SEPARATION_IN_MM = 1
    MAXIMUM_SEPARATION_IN_MM = 20
    STEP_IN_MM = 0.5
    N_STEPS = int( ( MAXIMUM_SEPARATION_IN_MM - MINIMUM_SEPARATION_IN_MM )/STEP_IN_MM ) + 1

    for i in range( 0, N_STEPS ):
        sep = MINIMUM_SEPARATION_IN_MM + i*STEP_IN_MM
        final_str = "/det/setZ LaBr " + str(sep+0.5) + " mm" # THIS ADDS THE HALF-THICKNESS OF THE RECOIL E SO THAT WE HAVE TRUE SEPARATION DISTANCE BETWEEN EDGE OF SI AND THE START OF LABR's
        file_name = "macros/LaBr_dist_" + str(sep).replace( ".", "_" ) + ".mac"
        sim_file_name_cmd = "/setOutputFileName results/LaBr_dist_" + str(sep).replace( ".", "_" )

        with open( file_name, "w" ) as file:
            file.write( "\n".join( GEOMETRY_FILE_STR_ARR ) )
            file.write( "\n" )
            file.write( final_str + "\n")
            file.write( sim_file_name_cmd )


if __name__ == "__main__":
    main()
