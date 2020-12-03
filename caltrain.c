#include "pintos_thread.h"

struct station {
    int cho_trong; // số lượng chỗ trống trên tàu 
    int hk_doi; // số lượng hành khách đang đợi chỗ trống
    int hk_tim_thay; // số lượng hành khách tìm thấy 1 chỗ trống
    struct lock lock;
    struct condition cho_trong_co_san; // tàu còn chỗ trống
    struct condition hk_tren_tau; // tất cả hành khách tìm thấy chỗ trống được đưa vào chỗ ngồi
};

void
station_init(struct station *station)
{
    /* khởi tạo station rỗng */
    station->cho_trong = 0;
    station->hk_doi = 0;
    station->hk_tim_thay = 0;
    /* khởi tạo lock and condition variables của station*/
    lock_init(&(station->lock));
    cond_init(&(station->cho_trong_co_san));
    cond_init(&(station->hk_tren_tau));
}

void
station_load_train(struct station *station, int Count)
{
    lock_acquire(&(station->lock)); 
    if (Count==0 || station->hk_doi==0) { // không có chỗ trống hoặc không có hành khách đợi lên tàu
        lock_release(&(station->lock)); 
        return; // tàu rời khỏi ga
    }
    station->cho_trong= Count;
    cond_broadcast(&(station->cho_trong_co_san),&(station->lock)); // báo hiệu tất cả hành khách đang đợi rằng có chỗ trống
    cond_wait(&(station->hk_tren_tau), &(station->lock)); // đợi tất cả hành khách lên tàu
    station->cho_trong = 0;
    lock_release(&(station->lock)); 
}

void
station_wait_for_train(struct station *station)
{
    lock_acquire(&(station->lock)); 
    station->hk_doi++;
    while (station->cho_trong==0)
       {
           cond_wait(&(station->cho_trong_co_san), &(station->lock)); // đợi tàu có chỗ trống
       }
    station->hk_doi--;
    station->hk_tim_thay++;
    station->cho_trong--;
    lock_release(&(station->lock)); 
}

void
station_on_board(struct station *station)
{
    lock_acquire(&(station->lock));  
    station->hk_tim_thay--;
    if (station->hk_tim_thay==0 && (station->hk_doi==0 || station->cho_trong==0)) /* tất cả hành khách tìm thấy chỗ trống đã ở trên tàu và không có hành khách nào khác có thể lên tàu */
      {
        cond_signal(&(station->hk_tren_tau),&(station->lock));
      }
    lock_release(&(station->lock)); 
}
