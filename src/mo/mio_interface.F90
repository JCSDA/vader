!
! (C) Crown Copyright 2022 Met Office
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
 module mio_data

 implicit none

 private
 public :: c_umGetLookUp2D

 contains

 !-------------------------------------------------------------------
 subroutine err_report(err_value, str_mess)

   !use unifiedmodel_constants_mod, only : error_string_length
   integer, parameter               :: error_string_length=800

   integer,          intent(in) :: err_value
   character(len=*), intent(in) :: str_mess

   ! for jedi error handling
   character(len=error_string_length) :: err_msg

   if (err_value /= 0) then
     write(err_msg,"(2a,i0)") trim(str_mess), " failed with error code ", err_value
     call abor1_ftn(err_msg)
   end if

 end subroutine err_report
 !-------------------------------------------------------------------
 subroutine c_umGetLookUp2D(filename_length, c_filename, &
                        & fieldname_length, c_fieldname, nbins, nlevels, &
                        & values) bind(c, name="umGetLookUp2D_f90")

   use, intrinsic :: iso_c_binding, only: c_char, c_double, c_int
   use netcdf, only: &
     nf90_close, nf90_get_var, nf90_inq_varid, nf90_nowrite, nf90_open
   use string_f_c_mod, only: c_f_string
   implicit none

   integer, parameter :: char_length = 800

   integer(c_int),                intent(in)    :: filename_length
   character(kind=c_char, len=1), intent(in)    :: c_filename(filename_length+1)
   integer(c_int),                intent(in)    :: fieldname_length
   character(kind=c_char, len=1), intent(in)    :: c_fieldname(fieldname_length+1)
   integer(c_int),                intent(in)    :: nbins
   integer(c_int),                intent(in)    :: nlevels
   real(c_double),                intent(inout) :: values(nlevels,nbins)

   character(len=char_length) :: filename
   character(len=char_length) :: fieldname

   integer :: nc
   integer(c_int) :: ncid,  varid


   call c_f_string(c_filename, filename)
   call c_f_string(c_fieldname, fieldname)


   nc = nf90_open(filename, nf90_nowrite, ncid)
   call err_report(nc, "nf90_open "//trim(filename) )

   nc = nf90_inq_varid(ncid, fieldname, varid)
   call err_report(nc, "nf90_inq_varid "//trim(fieldname) )

   nc = nf90_get_var(ncid, varid, values)
   call err_report(nc, "nf90_get_var ")

   nc = nf90_close(ncid)
   call err_report(nc, "nf90_close ")

end subroutine c_umGetLookUp2D
!-------------------------------------------------------------------
end module mio_data
