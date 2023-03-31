#include <chrono>
#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>

struct V
{
    int v_id;//index
    double x;
    double y;
};
struct E
{
    int e_id;//index
    int V1; //vertex1 index
    int V2; //vertex2 index
};
struct F
{
    int f_id;//index
    std::vector<int> es; // edge index
    std::vector<int> vs; // vertex index
};
struct FV
{
    int cb_f_id;//created by which face
    double x;
    double y;
}
struct EV
{
    int cb_e_id;//created by which edge
    double x;
    double y;
}
struct VV
{
    int fo_v_id;//id for old vertex
    double x;
    double y;
}
struct cmpforv
{
    bool operator()(const V& a, const V& b)
    {
        return a.v_id < b.v_id;
    }
}
struct cmpfore
{
    bool operator()(const E& a, const E& b)
    {
        return a.e_id < b.e_id;
    }
}
struct cmpforf
{
    bool operator()(const F& a, const F& b)
    {
        return a.f_id < b.f_id;
    }
}

std::vector<V> vs; // vertex
std::vector<E> es; // edge
std::vector<F> fs; // face

std::vector<FV> new_face_vertex;//calculate face vertex
std::vector<EV> new_edge_vertex;//calculate average edge vertex
std::vector<VV> new_vertex;//update vertex
std::vector<E> new_edge;
std::vector<F> new_face;
std::vector<V> new_new_vertex;


int main()
{
    int m = 1;//iteration times
    std::string filename = "data.obj";
    std::ifstream file;
    file.open("data.obj");
    if (!file.is_open())
    {
        std::cout << "Failed to open file: " << filename << std::endl;
        return 1;
    }
    std::string line;
    int vid = 1, eid = 1, fid = 1;
    while (getline(file, line)) // read from .obj
    {
        if (line.substr(0, 2) == "v ")
        {
            std::istringstream s(line.substr(2));
            double x, y;
            s >> x >> y;
            V v;
            v.v_id = vid;
            v.x = x;
            v.y = y;
            vid++;
            vs.push_back(v);
        }
        else if (line.substr(0, 2) == "e ")
        {
            std::istringstream s(line.substr(2));
            int first, second;
            s >> first >> second;
            E e;
            e.e_id = eid;
            e.V1 = first;
            e.V2 = second;
            eid++;
            es.push_back(e);
        }
        else if (line.substr(0, 2) == "f ")
        {
            std::istringstream s(line.substr(2));
            int first, second, third, fourth;
            s >> first >> second >> third >> fourth;
            F f;
            f.f_id = fid;
            f.vs.push_back(first);
            f.vs.push_back(second);
            f.vs.push_back(third);
            if (fourth != 0)
                f.vs.push_back(fourth);
            fid++;
            for (int i = 0; i <= f.vs.size() - 1; i++)
            {
                int one = f.vs[i], two = f.vs[(i + 1) % f.vs.size()];
                for (int j = 0; j <= es.size() - 1; j++)
                {
                    if ((es[j].V1 == one && es[j].V2 == two) || (es[j].V1 == two && es[j].V2 == one))
                    {
                        f.es.push_back(es[j].e_id);
                    }
                }
            }
            fs.push_back(f);
        }
    }
    file.close();
    while (m--)//start iteration
    {
        //face vertex
        for (int i = 0; i <= fs.size() - 1; i++)
        {
            FV nfv;
            nfv.cb_f_id = fs[i].f_id;
            nfv.x = 0;
            nfv.y = 0;
            for (int j = 0; j <= fs[i].vs.size() - 1; j++)
            {
                nfv.x = nfv.x + vs[fs[i].vs[j] - 1].x / fs[i].vs.size();
                nfv.y = nfv.y + vs[fs[i].vs[j] - 1].y / fs[i].vs.size();
            }
            new_face_vertex.push_back(nfv);
        }
        //edge vertex
        for (int i = 0; i <= es.size() - 1; i++)
        {
            int count = 0;//neighbour face num
            int fa[2];
            EV nev;
            nev.cb_e_id = es[i].e_id;
            nev.x = 0;
            nev.y = 0;
            for (int j = 0; j <= fs.size() - 1; j++)
            {
                for (int k = 0; k <= fs[j].es.size() - 1; k++)
                {
                    if (fs[j].es[k] == nev.cb_e_id)
                    {
                        fa[count] = fs[j].f_id;
                        count++;
                    }
                }
            }
            if (count == 1)//edge at edge
            {
                nev.x = (vs[es[i].V1 - 1].x + vs[es[i].V2 - 1].x) / 2;
                nev.y = (vs[es[i].V1 - 1].y + vs[es[i].V2 - 1].y) / 2;
            }
            else//edge with 2 neighbour faces
            {
                nev.x = (vs[es[i].V1 - 1].x + vs[es[i].V2 - 1].x + new_face_vertex[fa[0] - 1].x + new_face_vertex[fa[1] - 1].x) / 4;
                nev.y = (vs[es[i].V1 - 1].y + vs[es[i].V2 - 1].y + new_face_vertex[fa[0] - 1].y + new_face_vertex[fa[1] - 1].y) / 4;
            }
            new_edge_vertex.push_back(nev);
        }
        //update vertex
        for (int i = 0; i <= vs.size() - 1; i++)
        {
            VV nvv;
            nvv.fo_v_id = vs[i].v_id;
            nvv.x = 0;
            nvv.y = 0;
            int count = 0;//vertex degree
            int vf[5];
            int ve[5];
            for (int j = 0; j <= fs.size() - 1; j++)
            {
                for (int k = 0; k <= fs[j].vs.size() - 1; k++)
                {
                    if (fs[j].vs[k] == nvv.fo_v_id)
                    {
                        vf[count] = fs[j].f_id;
                        count++;
                    }
                }
            }
            count = 0;
            for (int j = 0; j <= es.size() - 1; j++)
            {
                if (es[j].V1 == nvv.fo_v_id || es[j].V2 == nvv.fo_v_id)
                {
                    ve[count] = es[j].e_id;
                    count++;
                }
            }
            if (count == 4)
            {
                nvv.x += vs[nvv.fo_v_id - 1].x / 4;
                nvv.y += vs[nvv.fo_v_id - 1].y / 4;
                for (int j = 0; j <= 3; j++)
                {
                    nvv.x = nvv.x + new_face_vertex[vf[j] - 1].x / 16 + new_edge_vertex[ve[j] - 1].x / 8;
                    nvv.y = nvv.y + new_face_vertex[vf[j] - 1].y / 16 + new_edge_vertex[ve[j] - 1].y / 8;
                }
            }
            else if (count == 5)
            {
                nvv.x += vs[nvv.fo_v_id - 1].x / 4;
                nvv.y += vs[nvv.fo_v_id - 1].y / 4;
                for (int j = 0; j <= 4; j++)
                {
                    nvv.x = nvv.x + new_face_vertex[vf[j] - 1].x / 20 + new_edge_vertex[ve[j] - 1].x / 10;
                    nvv.y = nvv.y + new_face_vertex[vf[j] - 1].y / 20 + new_edge_vertex[ve[j] - 1].y / 10;
                }
            }
            else//vertex at boundary or corner
            {
                nvv.x = vs[nvv.fo_v_id - 1].x;
                nvv.y = vs[nvv.fo_v_id - 1].y;
            }
            new_vertex.push_back(nvv);
        }
        //new vertexs
        int nvn = 1;//num for new vertex
        for (int i = 0; i <= new_face_vertex.size() - 1; i++)
        {
            V nvv;
            nvv.v_id = nvn;
            nvv.x = new_face_vertex[i].x;
            nvv.y = new_face_vertex[i].y;
            new_new_vertex.push_back(nvv);
            nvn++;
        }
        for (int i = 0; i <= new_edge_vertex.size() - 1; i++)
        {
            V nvv;
            nvv.v_id = nvn;
            nvv.x = new_edge_vertex[i].x;
            nvv.y = new_edge_vertex[i].y;
            new_new_vertex.push_back(nvv);
            nvn++;
        }
        for (int i = 0; i <= new_vertex.size() - 1; i++)
        {
            V nvv;
            nvv.v_id = nvn;
            nvv.x = new_vertex[i].x;
            nvv.y = new_vertex[i].y;
            new_new_vertex.push_back(nvv);
            nvn++;
        }
        std::sort(vs.begin(), vs.end(), cmpforv());
        //new edges
        int nen = 1;// num for new edges
        for (int i = 0; i <= fs.size() - 1; i++)
        {
            for (int j = 0; j <= fs[i].es.size() - 1; j++)
            {
                E nee1, nee2, neef;
                nee1.e_id = nen;
                nen++;
                nee2.e_id = nen;
                nen++;
                neef.e_id == nen;
                nen++;
                //under construction
            }
        }
        std::sort(es.begin(), es.end(), cmpfore());
        //new faces

        std::sort(fs.begin(), fs.end(), cmpforf());

        //copy

    }
    return 0;
}