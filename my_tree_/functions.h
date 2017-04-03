
B comp(pair<I,F> i,pair<I,F> j) { return (i.second<j.second); }


void split(const S &s, C delim, vector<D> &elems) {
    std::stringstream ss;
    ss.str(s);
    S item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(stod(item));

    }

}

D euclidean(vector<D>& v1, vector<D>&v2){
    D cont=0;
    for(tcont i=0;i<v1.size();++i){
        cont+=pow(v2[i]-v1[i],2);
    }
    return sqrt(cont);
}

S to_string(vector<D> &h){
    S aux="";
    for (auto i:h){
        aux+=std::to_string(i);
    }
    return aux;
}

