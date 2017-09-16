//
// Created by radmi on 06.09.2017.
//
#include "MeshLoader.h"

void MeshLoader::OutData() {
    int i = 1;
    std::cout << "Nodes:" << std::endl;
    forEach(s_node.begin(), s_node.end(), [&i](Node el){std::cout << std::setw(4) << el.ID << ": ";
        forEach(el.coords.begin(), el.coords.end(), [](double& d){std::cout << std::setw(9) << std::fixed << std::setprecision(4)
                                                                            << d << ' ';}); std::cout << std::endl;});
    std::cout << "Elements:" << std::endl;
    forEach(s_elem.begin(), s_elem.end(), [&i](Element el){std::cout << std::setw(4) << i++ << ": " << std::setw(4) << el.material_ID << ' ';
    forEach(el.ID_node.begin(), el.ID_node.end(),[](int& i){std::cout << std::setw(9) << i++ << ' ';}); std::cout << std::endl;});
    std::cout << "Surfaces:" << std::endl;
    i = 1;
    forEach(s_surf.begin(), s_surf.end(), [&i](Surface sr){std::cout << std::setw(4) << i++ << ": " << std::setw(4) << sr.ID_surf_cond << ' ';
    forEach(sr.ID_node.begin(), sr.ID_node.end(), [](int& i){std::cout<< std::setw(9) << i++ << ' ';}); std::cout << std::endl;});
}

std::vector<Node> MeshLoader::GetNode() {
    return s_node;
}

std::vector<Element> MeshLoader::GetElement() {
    return s_elem;
}

std::vector<Surface> MeshLoader::GetSurface() {
    return s_surf;
}

class Predicate {
    int* ID;
    int size;
    bool NextSet(int *a, int n, int m) {
        int j;
        do {
            j = n - 1;
            while (j != -1 && a[j] >= a[j + 1]) j--;
            if (j == -1)
                return false;
            int k = n - 1;
            while (a[j] >= a[k]) k--;
            std::swap(a[j], a[k]);
            int l = j + 1, r = n - 1;
            while (l < r)
                std::swap(a[l++], a[r--]);
        } while (j > m - 1);
        return true;
    }
    bool Find(int *a, int n, Element* el) {
        for (int i = 0; i < n; i++)
            if(ID[i] != (*el).ID_node[a[i]]) return false;
        return true;
    }
public:
    Predicate(int* ID_p, int size_p){
        ID = ID_p;
        size = size_p;
    }
    bool operator()(Element& el_p) {
        int *a;
        int n = el_p.ID_node.size();
        int m = size;
        a = new int[n];
        for (int i = 0; i < n; i++)
            a[i] = i;
        if(Find(a, m, &el_p)) return true;
        while (NextSet(a, n, m))
            if(Find(a, m, &el_p)) return true;
        return false;
    }
};

std::vector<Element> MeshLoader::FindElements(int *ID, int size) {
    std::vector<Element> find_el;
    auto el = s_elem.begin();
    do {
        el = FindIf(el, s_elem.end(), Predicate(ID, size));
        if(s_elem.end() != el) {find_el.push_back(*el); ++el;}
        else break;
    } while(1);
    return  find_el;
}

std::vector<Element> MeshLoader::FindElemIDmater(int ID_material) {
    std::vector<Element> find_el;
    CopyIf(s_elem.begin(), s_elem.end(), find_el.begin(), [&ID_material](Element& el){if(ID_material == el.material_ID) return true; return false;});
    return find_el;
}

std::vector<Surface> MeshLoader::FindSurfIDcond(int IDcond) {
    std::vector<Surface> find_sr;
    CopyIf(s_surf.begin(), s_surf.end(), find_sr.begin(), [&IDcond](Surface& sr){if(IDcond == sr.ID_surf_cond) return true; return false;});
    return find_sr;
}

std::vector<Node> MeshLoader::FindNodeIDcond(int IDcond) {
    std::vector<Node> find_nd;
    forEach(s_surf.begin(), s_surf.end(), [&](Surface& sr){if(sr.ID_surf_cond == IDcond)
        for(int i = 0; i < sr.ID_node.size(); ++i) {
            bool key = true;
            for(int j = 0; j < find_nd.size(); ++j)
                if(find_nd[j].ID == sr.ID_node[i]) key = false;
            if(key) find_nd.push_back(s_node[sr.ID_node[i] - 1]);}});
    return find_nd;
}

std::vector<std::pair<Node, std::vector<Node>>> MeshLoader::NeighborElementEach() {
    std::vector<std::pair<Node, std::vector<Node>>> res;
    bool sear;
    bool key;
    std::vector<Node> tmp;
    forEach(s_node.begin(), s_node.end(), [&](Node& nd) {
        forEach(s_elem.begin(), s_elem.end(), [&](Element& el) {
            key = false;
            for(int i = 0; i < el.ID_node.size(); ++i)
                if(el.ID_node[i] == nd.ID) {key = true; break;}
            if(key) {
                for(int i = 0; i < el.ID_node.size(); ++i) {
                    sear = true;
                    if(el.ID_node[i] != nd.ID) {
                        for (int j = 0; j < tmp.size(); ++j)
                            if (tmp[j].ID == el.ID_node[i]) sear = false;
                    } else {
                        sear = false;
                    }
                    if(sear) tmp.push_back(s_node[el.ID_node[i] - 1]);
                }
            }
        });
        res.push_back(std::make_pair(nd, tmp));
        tmp.clear();});
    return res;
}
