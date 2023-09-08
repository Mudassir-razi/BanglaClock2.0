xoffset = 0;
yoffset = 0;
scaleFactor = 14;
cursorList = [1 1.5 2 1] * scaleFactor;

indexer = ' ';
bitmap = ' ';

defaultData = sprintf('0, 0, 0, 5,');
imageNames = "sNBrISm@lUdvhPTuCiQYjqAXRWbnKLkoGHeMzfgcOwtpayxV1234567890%{{DEFJZ}!";

letterGroup1 = "vsmldhYbnhoezOpxUgyPuCRG";
letterGroup2 = "NBITiQjqAXWKLHMfctaVD";
letterGroup3 = "SwE";

asciiStart = 32;
asciiEnd = 126;
offset = 0;
hexFile = fopen('C:\Users\koomf\Documents\Arduino\libraries\bitluni_ESP32Lib\src\Ressources\CustomBangla.h', 'w');

for i = asciiStart: asciiEnd
    
    currentLetter = char(i);
    %determining letter offset
    lg = 4;
    if contains(letterGroup1, currentLetter)
        lg = 1;
    elseif contains(letterGroup2, currentLetter)
        lg = 2;
    elseif contains(letterGroup3, currentLetter)
        lg = 3;
    end
    %

    if contains(imageNames, currentLetter)
        %push data for this character
        letterName = char(i);
        if i >= 'A' && i <= 'Z'
            letterName = letterName + "_";
        end
        
        letter = imread("output\" + letterName + ".png");
        data = sprintf(' %d, %d, %d, %d,', offset, width(letter), height(letter), cursorList(lg));
        indexer = sprintf('%s \n %s', indexer, data);
        offset = offset + (width(letter) * height(letter));
        
        %generating the bitmap 
        %letter = letter';
        letter = abs(1 - letter);
        tempBit  = '';
        singleBitmap = '';
        for k = 1:length(letter)
            tempBit = sprintf('%d,',letter(k, :));
            singleBitmap = sprintf('%s\n%s',singleBitmap, tempBit);
        end
        %letter = reshape(letter, [1, width(letter) * height(letter)]);
        %
        
        %tempBit = sprintf('%d,', letter);
        %tempBit = sprintf('%s\n', tempBit);
        bitmap = sprintf('%s\n\n%s', bitmap, singleBitmap);
    else
        %push default data
        indexer = sprintf('%s \n %s', indexer, defaultData);
    end
end

bitmap = strtrim(bitmap);
bitmap = bitmap(1:end-1);

indexer = strtrim(indexer);
indexer = indexer(1:end-1);

fprintf(hexFile, 'const unsigned char pixels [] = { %s };\n const int data[] = { %s };\nFontVar customBangla(8, 8, pixels, data, 32, 120);', bitmap, indexer);

fclose(hexFile);

