import json
import sys

def main(argv):
    if len(argv) >= 2 and argv[1] in ("--help", "-h"):
        print("use like this: %s INFILE OUTFILE" % argv[0])
    elif len(argv) < 3:
        print("Too few arguments. Need --help?")
        sys.exit(1)
    else:
        with open(argv[1]) as asesheet:
            sheetdata = json.load(asesheet)
            carray = open(argv[2], "wt")
            cheader = open((argv[2].split(".")[0]+".h"),"wt")
            
            cheader.write("#pragma once\n")
            carray.write("#include <stdio.h>\n")
            cheader.write("#include <stdio.h>\n")
            carray.write("#include <tonc.h>\n")
            cheader.write("#include <tonc.h>\n")
            carray.write("#include \"main.h\"\n")
            cheader.write("#include \"main.h\"\n")
            
            for i in range(len(sheetdata["frames"])):
                tobecount = 1
                carraytobe = []
                carraytobe += ["const u16 " + sheetdata["frames"][i]["filename"].replace(" ","_").replace("#","_") + "[] = {\n"]
                carraytobeh = []
                carraytobev = []
                carraytobehv = []
                print("frame " + str(i))
                spritestepv = 64
                j = 0
                je = sheetdata["frames"][i]["spriteSourceSize"]["h"] - sheetdata["frames"][i]["spriteSourceSize"]["y"]
                jhw = 0
                while j < sheetdata["frames"][i]["spriteSourceSize"]["h"]:
                    if 32 <= (sheetdata["frames"][i]["spriteSourceSize"]["h"] - j) < 64:
                        spritestepv = 32
                    elif 16 <= (sheetdata["frames"][i]["spriteSourceSize"]["h"] - j) < 32:
                        spritestepv = 16
                    elif 8 <= (sheetdata["frames"][i]["spriteSourceSize"]["h"] - j) < 16:
                        spritestepv = 8
                        
                    je -= spritestepv
                    
                    print("y " + str(spritestepv))
                    
                    if spritestepv > 16:
                        spritesteph = 64
                    else:
                        spritesteph = 32
                    k = 0
                    ke = sheetdata["frames"][i]["spriteSourceSize"]["w"] - sheetdata["frames"][i]["spriteSourceSize"]["x"]
                    khw = 0
                    while k < sheetdata["frames"][i]["spriteSourceSize"]["w"]:
                        if 32 <= (sheetdata["frames"][i]["spriteSourceSize"]["w"] - k) < 64:
                            spritesteph = 32
                        elif 16 <= (sheetdata["frames"][i]["spriteSourceSize"]["w"] - k) < 32:
                            spritesteph = 16
                        elif 8 <= (sheetdata["frames"][i]["spriteSourceSize"]["w"] - k) < 16:
                            spritesteph = 8
                        spriteshape = spritesteph / spritestepv
                        if spriteshape > 1:
                            spriteshape = "(ATTR0_WIDE >> 2)"
                        elif spriteshape < 1:
                            spriteshape = "(ATTR0_TALL >> 2)"
                        else:
                            spriteshape = "(ATTR0_SQUARE >> 2)"
                        ke -= spritesteph
                        print(str(spritesteph) + " by " + str(spritestepv))
                        carraytobe += [str(j + sheetdata["frames"][i]["spriteSourceSize"]["y"]) + ", " + str(k + sheetdata["frames"][i]["spriteSourceSize"]["x"]) + ", " + str(round(((sheetdata["frames"][i]["frame"]["y"] + j) * 4) + (sheetdata["frames"][i]["frame"]["x"] + k) / 8)) + ","  + spriteshape + " | ATTR1_SIZE_" + str(spritesteph) + "x" + str(spritestepv) + ",\n"]
                        carraytobeh += [str(j + sheetdata["frames"][i]["spriteSourceSize"]["y"]) + ", " + str(ke + sheetdata["frames"][i]["spriteSourceSize"]["x"]) + ", " + str(round(((sheetdata["frames"][i]["frame"]["y"] + j) * 4) + (sheetdata["frames"][i]["frame"]["x"] + ke) / 8)) + ","  + spriteshape + " | ATTR1_SIZE_" + str(spritesteph) + "x" + str(spritestepv) + ",\n"]
                        carraytobev += [str(je + sheetdata["frames"][i]["spriteSourceSize"]["y"]) + ", " + str(k + sheetdata["frames"][i]["spriteSourceSize"]["x"]) + ", " + str(round(((sheetdata["frames"][i]["frame"]["y"] + je) * 4) + (sheetdata["frames"][i]["frame"]["x"] + k) / 8)) + ","  + spriteshape + " | ATTR1_SIZE_" + str(spritesteph) + "x" + str(spritestepv) + ",\n"]
                        tobecount += 12
                        k += spritesteph
                        khw += 4
                    j += spritestepv
                    jhw += 1
                tobecount += 3
                carraytobe += ["-1,\n"]
                carraytobeh += ["-1,\n"]
                carraytobev += ["-1,\n"]
                carraytobe[0] = "const s16 " + sheetdata["frames"][i]["filename"].replace(" ","_").replace("#","_") + "[" + str(tobecount) + "] = {\n" + str((jhw*khw)+1) + ",\n"
                
                cheader.write("extern const s16 " + sheetdata["frames"][i]["filename"].replace(" ","_").replace("#","_") + "[" + str(tobecount) + "];\n")
                carray.write(" ".join(carraytobe) + " ".join(carraytobeh) + " ".join(carraytobev) + " ".join(carraytobehv) + "};\n")
                
            for i in range(len(sheetdata["meta"]["frameTags"])):
                carray.write("const struct spranim_data " + sheetdata["meta"]["frameTags"][i]["name"].replace(" ","_") + "_anim[" + str((sheetdata["meta"]["frameTags"][i]["to"] - sheetdata["meta"]["frameTags"][i]["from"]) + 2) + "] = {\n")
                cheader.write("extern const struct spranim_data " + sheetdata["meta"]["frameTags"][i]["name"].replace(" ","_") + "_anim[" + str((sheetdata["meta"]["frameTags"][i]["to"] - sheetdata["meta"]["frameTags"][i]["from"]) + 2) + "];\n")
                for j in range(sheetdata["meta"]["frameTags"][i]["from"], sheetdata["meta"]["frameTags"][i]["to"] + 1):
                    carray.write("{" + sheetdata["frames"][j]["filename"].replace(" ","_").replace("#","_") + ", ")
                    if (sheetdata["meta"]["frameTags"][i]["to"] + 1) - sheetdata["meta"]["frameTags"][i]["from"] > 1:
                        carray.write(str(round(sheetdata["frames"][j]["duration"]/(1000/60))) + "},\n")
                    else:
                        carray.write("-1},\n")
                
                carray.write("{" + sheetdata["frames"][0]["filename"].replace(" ","_").replace("#","_") +", 0}\n};\n")
                
            carray.close()
            cheader.close()

if __name__=='__main__':
    main(sys.argv)