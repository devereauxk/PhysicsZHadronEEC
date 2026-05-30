// appendSubbinKeys.C
// Usage: root -l -b -q 'appendSubbinKeys.C("target.root","source.root")'
// Copies every key from source that is not already present in target.
void appendSubbinKeys(const char* targetPath, const char* sourcePath)
{
    TFile* target = TFile::Open(targetPath, "UPDATE");
    if (!target || target->IsZombie()) {
        fprintf(stderr, "Cannot open target: %s\n", targetPath);
        return;
    }
    TFile* source = TFile::Open(sourcePath, "READ");
    if (!source || source->IsZombie()) {
        fprintf(stderr, "Cannot open source: %s\n", sourcePath);
        target->Close();
        return;
    }

    TIter next(source->GetListOfKeys());
    TKey* key;
    int nCopied = 0;
    while ((key = (TKey*)next())) {
        const char* name = key->GetName();
        if (target->FindKey(name)) continue;   // already present, skip
        TObject* obj = key->ReadObj();
        if (!obj) continue;
        target->cd();
        obj->Write(name);
        delete obj;
        nCopied++;
    }
    printf("Appended %d new keys from %s -> %s\n", nCopied, sourcePath, targetPath);
    target->Close();
    source->Close();
}
