#pragma comment(lib,"heif-master/build/lib/Debug/heif_shared.lib")

#include <iostream>
#include "heifreader.h"

using namespace std;
using namespace HEIF;

int main() {
    auto* reader = Reader::Create();

    char filename[256];

    cin >> filename;
    //scanf_s("%s", filename);

    if (reader->initialize(filename) == ErrorCode::OK)
    {
        cout << "File input: " << filename << ". " << endl;
    }
    else {
        cout << "Can't input file: " << filename << ". " << endl;
        return 1;
    }

    FileInformation fileInfo{};
    reader->getFileInformation(fileInfo);

    // Find the primary item ID.
    ImageId primaryItemId;
    reader->getPrimaryItem(primaryItemId);

    // Find item(s) referencing to the primary item with "cdsc" (content describes) item reference.
    Array<ImageId> metadataIds;
    reader->getReferencedToItemListByType(primaryItemId, "cdsc", metadataIds);

    for (ImageId metadataId : metadataIds) {
        cout << metadataId.get() << endl;
    }

    for (ItemInformation itemInfo : fileInfo.rootMetaBoxInformation.itemInformations)
    {
        ImageId itemId = itemInfo.itemId;
        FourCC type = itemInfo.type;

        ItemDescription description = itemInfo.description;

        FeatureBitMask features = itemInfo.features;
        uint64_t size = itemInfo.size;

        if (type == "hvc1") {
            continue;
        }

        uint8_t* buf = new uint8_t[size];
        ErrorCode result = reader->getItemData(itemId, buf, size);

        

        cout << itemId.get() << " - " << size << ", " << type.value << ", ";
        cout << ((features & ItemFeatureEnum::IsExifItem) ? "Exif" : ((features & ItemFeatureEnum::IsXMPItem) ? "XMP" : ((features & ItemFeatureEnum::IsMPEG7Item) ? "MPEG7" : "Unknown")));
        cout << endl;

        cout << "\tname : " << description.name.elements << endl;
        cout << "\ttype : " << description.contentType.elements << endl;
        cout << "\tencoding : " << description.contentEncoding.elements << endl;

        if (result == ErrorCode::OK) {
            cout << "\tdata : " << reinterpret_cast<char*>(buf) << endl << endl;
        }
        else {
            cout << "\tdata failed : ErrorCode " << (int)result << endl << endl;
        }

        delete[] buf;
    }

    /*ImageId exifItemId = metadataIds[0];

    // Optional: verify the item ID we got is really of "Exif" type.
    FourCC itemType;
    reader->getItemType(exifItemId, itemType);
    if (itemType != "Exif")
    {
        return 2;
    }

    

    // Get item size from parsed information. For simplicity, assume it is the first and only non-image item in the
    // file.
    if (fileInfo.rootMetaBoxInformation.itemInformations[0].itemId != exifItemId)
    {
        return 3;
    }
    auto itemSize = fileInfo.rootMetaBoxInformation.itemInformations[0].size;

    // Request item data.
    uint8_t* memoryBuffer = new uint8_t[itemSize];
    reader->getItemData(metadataIds[0], memoryBuffer, itemSize);*/

    reader->close();

    reader->initialize(filename);

    Reader::Destroy(reader);

    //reader->initialize(filename);

    //Reader::Destroy(reader);

    return 0;
}